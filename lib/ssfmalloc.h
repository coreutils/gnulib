/* Simple and straight-forward malloc implementation (front end).

   Copyright (C) 2020-2022 Free Software Foundation, Inc.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of the
   License, or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Written by Bruno Haible <bruno@clisp.org>, 2020.  */

/* This file implements an allocator of memory blocks of given size (a
   "malloc front end"), based on an allocator of memory pages (a "malloc
   back end").

   The need for such an allocator arises because a memory block is often
   50 bytes large or less, whereas an allocator of memory pages provides
   entire pages (4096 bytes or more).

   This implementation attempts to be
     - simple and straight-forward,
     - respecting locality of reference,
     - usable for small allocations,
     - nevertheless of reasonable speed.

   Simple and straight-forward - means that it contains only a small amount
   of code (compared to e.g. tcmalloc).

   Respecting locality of reference - means that searching for a free block
   will not follow lists of pointers that touch unrelated cache lines in
   the same page or even unrelated memory pages, because that would cause
   cache misses or even swapping in of unrelated memory pages.

   Usable for small allocations - means that it can be used for data types
   with few instances.  It does not, unlike some other malloc implementations,
   allocate 256 KB of memory up-front.  Nor does it allocate memory pages
   per thread.

   Reasonable speed is nevertheless guaranteed by
     - choosing algorithms that lead to little fragmentation,
     - small caches where they make sense.
 */

/* The user of this file needs to define the following macros before including
   this file:

     PAGESIZE           A variable-like macro of type intptr_t or uintptr_t
                        that evaluates to the memory page size (>= 4096).

     PAGESIZE_MAX       A constant that specifies an upper bound for PAGESIZE.

     ALLOC_PAGES        A function-like macro with the signature
                          uintptr_t ALLOC_PAGES (size_t size)
                        where the argument size is > 0 and a multiple of the
                        PAGESIZE.  It returns a multiple of PAGESIZE, or 0
                        upon failure.

     FREE_PAGES         A function-like macro with the signature
                          void FREE_PAGES (uintptr_t pages, size_t size)
                        where pages is a non-zero value returned by
                        ALLOC_PAGES (size).

     ALIGNMENT          A constant that specifies the desired alignment of all
                        the returned memory blocks.  Possible values are the
                        powers of 2, from sizeof (void *) to 32.

     PAGE_RESERVED_HEADER_SIZE   A constant, either 0 or a multiple of
                        sizeof (void *), that denotes the size of a reserved
                        header - not to be used by the application - at the
                        beginning of a page sequence returned by ALLOC_PAGES.
 */

/* =================== Declarations of exported functions =================== */

#include <stdint.h>

/* Allocates a block of memory, aligned to ALIGNMENT bytes.
   Returns 0 upon failure.  */
static uintptr_t allocate_block (size_t size);

/* Frees a block of memory, returned by allocate_block.  */
static void free_block (uintptr_t block);

/* ============================= Implementation ============================= */

/* Outline of the implementation decisions (ID):

   * ID: This implementation considers three types of blocks:
     - small blocks - these are allocated in "small block" pages.
     - medium blocks - these are allocated in "medium block" pages.
     - large blocks - these are allocated individually, with a page or
       sequence of pages uniquely for this block.
   * Rationale:
     - Most memory allocations are small (e.g. <= 32 bytes); this is a lesson
       learned from xc/programs/Xserver/os/xalloc.c (1997) [Pascal Haible].
     - Fragmentation is one of the biggest problems, and keeping large
       blocks and small blocks separate from medium blocks is one way to
       control it.

   * ID: If an allocation succeeds in one page, the next allocation (of the
     same type of block) will try to use the same page.
   * Rationale: Locality of reference.

   * ID: Pages of small or medium blocks have their management data structures
     concentrated at the beginning of the page.  No chained lists that force
     to walk through the page.
   * Rationale: Locality of reference.

   * ID: Across pages, the management of the free space is done through data
     structures outside the pages.  No chained lists across pages.
   * Rationale: Locality of reference.

 */

#include <stdlib.h>
#include <string.h>     /* ffsll */
#include <strings.h>    /* ffs */
#include "flexmember.h"
#include "glthread/lock.h"
#include "thread-optim.h"
#include "gl_oset.h"
#include "gl_rbtree_oset.h"

/* Help the branch prediction.  */
#if __GNUC__ >= 3
# define likely(cond)   __builtin_expect ((cond), 1)
# define unlikely(cond) __builtin_expect ((cond), 0)
#else
# define likely(cond)   (cond)
# define unlikely(cond) (cond)
#endif

enum { small_page_type = 1, medium_page_type = 2, large_page_type = 3 };

/* Header of a page of small or medium blocks or of a large block.
   Lies at an address that is a multiple of PAGESIZE.  */
struct any_page_header
{
  #if PAGE_RESERVED_HEADER_SIZE > 0
  void * reserved[PAGE_RESERVED_HEADER_SIZE / sizeof (void *)];
  #endif
  /* small_page_type or medium_page_type or large_page_type */
  unsigned char page_type;
};

/* ========================= Small and medium blocks ======================== */

/* An integer type, capable of holding the values 0 .. PAGESIZE.  */
#if PAGESIZE_MAX >= 0x10000
typedef unsigned int   pg_offset_t;
#else
typedef unsigned short pg_offset_t;
#endif

/* Tree element that corresponds to a page.
   These tree elements are allocated via malloc().  */
struct page_tree_element
{
  uintptr_t page;
  pg_offset_t free_space;
};

/* Header of a page of small or medium blocks.
   Lies at an address that is a multiple of PAGESIZE.  */
struct dissected_page_header
{
  struct any_page_header common;
  /* Amount of free space in this page.  Always a multiple of ALIGNMENT.  */
  pg_offset_t free_space;
  /* The tree element.  */
  struct page_tree_element *tree_element;
};

/* Data structure for managing a pool of pages.  */
struct page_pool
{
  /* Methods.  */
  void (*init_page_pool) (struct page_pool *pool);
  void (*init_page) (uintptr_t page);
  uintptr_t (*allocate_block_in_page) (size_t size, uintptr_t page);
  void (*free_block_in_page) (uintptr_t block, uintptr_t page);

  /* Maximum free space in a page of this pool.  */
  size_t page_capacity;

  /* Page that provided the last successful allocation from this pool,
     or 0.  */
  uintptr_t last_page;

  /* Ordered set of managed pages, sorted according to free_space, in ascending
     order.  */
  gl_oset_t /* <struct page_tree_element *> */ managed_pages;

  /* A queue of pages which have a modified free_space but which have not been
     updated in the managed_pages tree so far.  */
  #define UPDATE_QUEUE_SIZE 10
  unsigned int update_queue_count; /* <= UPDATE_QUEUE_SIZE */
  uintptr_t update_queue[UPDATE_QUEUE_SIZE];

  /* A page that could be freed.
     We don't free it immediately, so that on allocation/deallocation
     pattern like
       2x allocate, 2x free, 2x allocate, 2x free, 2x allocate, 2x free, ...
     will not allocate and free a page so frequently.  */
  uintptr_t freeable_page;
};

/* Comparison function for managed_pages.  */
static int
compare_pages_by_free_space (const void *elt1, const void *elt2)
{
  struct page_tree_element *element1 = (struct page_tree_element *) elt1;
  struct page_tree_element *element2 = (struct page_tree_element *) elt2;
  int cmp = _GL_CMP (element1->free_space, element2->free_space);
  if (unlikely (cmp == 0))
    cmp = _GL_CMP (element1->page, element2->page);
  return cmp;
}

/* Tests whether the free space in a tree element is greater or equal than the
   given threshold.  */
static bool
page_free_space_is_at_least (const void *elt, const void *threshold)
{
  struct page_tree_element *element = (struct page_tree_element *) elt;

  return element->free_space >= (uintptr_t) threshold;
}

/* Updates the free space of a 'struct page_tree_element *'.
   Only to be called through gl_oset_update!  */
static void
set_free_space (const void *elt, void *action_data)
{
  struct page_tree_element *element = (struct page_tree_element *) elt;

  element->free_space = (pg_offset_t) (uintptr_t) action_data;
}

/* Executes the pending updates in the managed_pages tree.  */
static void
flush_all_updates (struct page_pool *pool)
{
  size_t count = pool->update_queue_count;
  while (likely (count > 0))
    {
      --count;
      uintptr_t page = pool->update_queue[count];
      struct dissected_page_header *pageptr =
        (struct dissected_page_header *) page;
      struct page_tree_element *tree_element = pageptr->tree_element;
      if (gl_oset_update (pool->managed_pages, tree_element,
                          set_free_space,
                          (void *) (uintptr_t) pageptr->free_space)
          < 0)
        /* A collision was found.  This contradicts the definition of
           compare_pages_by_free_space.  */
        abort ();
    }
  pool->update_queue_count = 0;
}

/* Adds a page to the update queue.
   This function has to be called when the free_space of the page has
   changed.  */
static inline void
add_update (uintptr_t page, struct page_pool *pool)
{
  size_t count = pool->update_queue_count;
  size_t i;
  for (i = 0; i < count; i++)
    if (pool->update_queue[i] == page)
      /* It's already in the queue.  */
      return;

  /* Ensure there is room for adding one more page to the update queue.  */
  if (unlikely (count == UPDATE_QUEUE_SIZE))
    flush_all_updates (pool);

  /* Add it to the update queue.  */
  pool->update_queue[pool->update_queue_count++] = page;
}

/* Drops a page from the update queue.  */
static inline void
drop_update (uintptr_t page, struct page_pool *pool)
{
  size_t count = pool->update_queue_count;
  size_t i;
  for (i = 0; i < count; i++)
    if (pool->update_queue[i] == page)
      {
        /* It's in the queue.  Remove it.  */
        for (i = i + 1; i < count; i++)
          pool->update_queue[i - 1] = pool->update_queue[i];
        pool->update_queue_count--;
        return;
      }
}

/* ============================== Small blocks ============================== */

#include "ssfmalloc-bitmap.h"

/* Maximum size of a small block.
   Must be a power of 2.  */
#define SMALL_BLOCK_MAX_SIZE (ALIGNMENT < 8 ? 32 * ALIGNMENT : 256)

/* Number of rows of ALIGNMENT bytes available in an empty page.  */
static unsigned int small_block_page_num_bits;
/* Offset in the page where the memory blocks start.
   A multiple of ALIGNMENT.  */
static unsigned int small_block_page_blocks_start;
/* Number of uint32_t words in each of the two bitmaps.  */
static unsigned int small_block_page_num_bitmap_words;

/* Header of a page of small blocks.
   Lies at an address that is a multiple of PAGESIZE.  */
struct small_page_header
{
  struct dissected_page_header common;
  /* Two bitmaps, each with small_block_page_num_bitmap_words. In each a bit
     represents ALIGNMENT bytes.
       - available_bitmap: bit set means available, bit clear means allocated.
       - blockend_bitmap: bit set means the an allocated block ends here.  */
  uint32_t bitmap_words[FLEXIBLE_ARRAY_MEMBER];
};

static inline uint32_t *
small_block_page_available_bitmap (struct small_page_header *pageptr)
{
  return &pageptr->bitmap_words[0];
}

static inline uint32_t *
small_block_page_blockend_bitmap (struct small_page_header *pageptr)
{
  return &pageptr->bitmap_words[small_block_page_num_bitmap_words];
}

static void
init_small_block_page_pool (struct page_pool *pool)
{
  /* How many usable rows of ALIGNMENT bytes can we have?
     Each takes ALIGNMENT bytes + 1/8 byte in each bitmap, so approximately
     (ALIGNMENT + 1/4) bytes.  */
  unsigned int num_bits = (unsigned int) (4 * PAGESIZE) / (4 * ALIGNMENT + 1);
  unsigned int num_bitmap_words;
  unsigned int blocks_start;
  /* Iterate until it converges.  */
  for (;;)
    {
      num_bitmap_words = (num_bits + 32 - 1) / 32;
      blocks_start =
        (FLEXSIZEOF (struct small_page_header, bitmap_words,
                     2 * num_bitmap_words * sizeof (uint32_t))
         + ALIGNMENT - 1) & -ALIGNMENT;
      unsigned int num_bits_r = (unsigned int) (PAGESIZE - blocks_start) / ALIGNMENT;
      if (num_bits_r >= num_bits)
        break;
      num_bits = num_bits_r;
    }

  small_block_page_num_bits         = num_bits;
  small_block_page_num_bitmap_words = num_bitmap_words;
  small_block_page_blocks_start     = blocks_start;

  pool->page_capacity = small_block_page_num_bits * ALIGNMENT;
}

static void
init_small_block_page (uintptr_t page)
{
  struct small_page_header *pageptr = (struct small_page_header *) page;
  pageptr->common.common.page_type = small_page_type;

  /* Initialize available_bitmap.  */
  uint32_t *available_bitmap = small_block_page_available_bitmap (pageptr);
  init_bitmap_all_bits_set (small_block_page_num_bitmap_words,
                            available_bitmap);
  if ((small_block_page_num_bits % 32) != 0)
    available_bitmap[small_block_page_num_bitmap_words - 1] =
      (1U << (small_block_page_num_bits % 32)) - 1;

  /* Initialize blockend_bitmap.  */
  init_bitmap_all_bits_clear (small_block_page_num_bitmap_words,
                              small_block_page_blockend_bitmap (pageptr));

  pageptr->common.free_space = small_block_page_num_bits * ALIGNMENT;
}

/* Allocates a block of memory of size <= SMALL_BLOCK_MAX_SIZE,
   aligned to ALIGNMENT bytes, from the given page.
   Returns 0 upon failure.  */
static uintptr_t
allocate_small_block_in_page (size_t size, uintptr_t page)
{
  struct small_page_header *pageptr = (struct small_page_header *) page;

  /* glibc compatible.  */
  if (size == 0)
    size = 1;

  /* Number of consecutive bits to look for in the bitmap.  */
  size_t c = (size + ALIGNMENT - 1) / ALIGNMENT;

  /* SMALL_BLOCK_MAX_SIZE has been chosen so that c <= 32.  */
  if (!(c > 0 && c <= 32))
    abort ();

  uint32_t *available_bitmap = small_block_page_available_bitmap (pageptr);
  size_t k = find_first_packet_set (small_block_page_num_bitmap_words,
                                    available_bitmap,
                                    c);
  if (unlikely (k == (size_t)(-1)))
    /* Failed to find c consecutive available rows of ALIGNMENT bytes each.  */
    return 0;

  uint32_t *blockend_bitmap = small_block_page_blockend_bitmap (pageptr);
  size_t j = k / 32;
  size_t i = k % 32;
  if (i + c <= 32)
    {
      available_bitmap[j] &= ~(((2U << (c - 1)) - 1) << i);
      blockend_bitmap[j] |= (1U << (i + c - 1));
    }
  else
    {
      available_bitmap[j] &= ~(-1U << i);
      available_bitmap[j + 1] &= ~((1U << (i + c - 32)) - 1);
      blockend_bitmap[j + 1] |= (1U << (i + c - 1 - 32));
    }

  pageptr->common.free_space -= c * ALIGNMENT;

  return page + small_block_page_blocks_start + k * ALIGNMENT;
}

static void
free_small_block_in_page (uintptr_t block, uintptr_t page)
{
  struct small_page_header *pageptr = (struct small_page_header *) page;

  if (!(block >= page + small_block_page_blocks_start
        && (block % ALIGNMENT) == 0))
    /* Invalid argument.  */
    abort ();

  uint32_t *available_bitmap = small_block_page_available_bitmap (pageptr);
  uint32_t *blockend_bitmap = small_block_page_blockend_bitmap (pageptr);

  /* The bit that corresponds to where the block starts.  */
  size_t k = (block - page - small_block_page_blocks_start) / ALIGNMENT;
  /* The bit that corresponds to where the block ends.  */
  size_t ke = find_first_bit_set (small_block_page_num_bitmap_words,
                                  blockend_bitmap,
                                  k);
  if (/* ke == (size_t)(-1) || */ ke >= k + 32)
    /* Invalid argument or invalid state.  */
    abort ();

  /* Number of consecutive bits to manipulate in the bitmap.  */
  size_t c = ke - k + 1;

  size_t j = k / 32;
  size_t i = k % 32;
  if (i + c <= 32)
    {
      available_bitmap[j] |= (((2U << (c - 1)) - 1) << i);
      blockend_bitmap[j] &= ~(1U << (i + c - 1));
    }
  else
    {
      available_bitmap[j] |= (-1U << i);
      available_bitmap[j + 1] |= ((1U << (i + c - 32)) - 1);
      blockend_bitmap[j + 1] &= ~(1U << (i + c - 1 - 32));
    }

  pageptr->common.free_space += c * ALIGNMENT;
}

/* Management of pages of small blocks.  */
struct page_pool small_block_pages =
  {
    init_small_block_page_pool,
    init_small_block_page,
    allocate_small_block_in_page,
    free_small_block_in_page
  };

/* ============================== Medium blocks ============================= */

/* A range of memory in a page.
   It covers the address range [page+start, page+end).
   start <= end.  */
struct memory_range
{
  pg_offset_t start;
  pg_offset_t end;
};

/* Header of a page of medium blocks.
   Lies at an address that is a multiple of PAGESIZE.  */
struct medium_page_header
{
  struct dissected_page_header common;
  /* If n blocks are allocated, there are n+1 gaps before, between, and
     after them.  Keep them in an array, sorted in ascending order.  */
  unsigned int num_gaps; /* > 0 */
  struct memory_range gaps[FLEXIBLE_ARRAY_MEMBER /* PAGESIZE / SMALL_BLOCK_MAX_SIZE + 1 */];
};

#define MEDIUM_BLOCKS_PAGE_MAX_GAPS \
  (PAGESIZE / SMALL_BLOCK_MAX_SIZE + 1)
#define MEDIUM_BLOCKS_PAGE_FIRST_GAP_START \
  ((FLEXSIZEOF (struct medium_page_header, gaps, \
                MEDIUM_BLOCKS_PAGE_MAX_GAPS * sizeof (struct memory_range)) \
    + ALIGNMENT - 1) & -ALIGNMENT)
#define MEDIUM_BLOCKS_PAGE_LAST_GAP_END \
  PAGESIZE
#define MEDIUM_BLOCKS_PAGE_CAPACITY \
  (MEDIUM_BLOCKS_PAGE_LAST_GAP_END - MEDIUM_BLOCKS_PAGE_FIRST_GAP_START)

static void
init_medium_block_page_pool (struct page_pool *pool)
{
  pool->page_capacity = MEDIUM_BLOCKS_PAGE_CAPACITY;
}

static void
init_medium_block_page (uintptr_t page)
{
  struct medium_page_header *pageptr = (struct medium_page_header *) page;
  pageptr->common.common.page_type = medium_page_type;
  pageptr->num_gaps = 1;
  pageptr->gaps[0].start = MEDIUM_BLOCKS_PAGE_FIRST_GAP_START;
  pageptr->gaps[0].end   = MEDIUM_BLOCKS_PAGE_LAST_GAP_END;
  pageptr->common.free_space = MEDIUM_BLOCKS_PAGE_CAPACITY;
}

/* Allocates a block of memory of size > SMALL_BLOCK_MAX_SIZE,
   aligned to ALIGNMENT bytes, from the given page.
   Returns 0 upon failure.  */
static uintptr_t
allocate_medium_block_in_page (size_t size, uintptr_t page)
{
  struct medium_page_header *pageptr = (struct medium_page_header *) page;

  /* Walk through the gaps and remember the smallest gap of at least
     the given size.  */
  size_t best_i = (size_t)(-1);
  size_t best_length = (size_t)(-1);
  size_t num_gaps = pageptr->num_gaps;
  size_t i;
  for (i = 0; i < num_gaps; i++)
    {
      size_t length = pageptr->gaps[i].end - pageptr->gaps[i].start;
      if (length >= size)
        {
          /* Found a gap of sufficient size.  */
          if (length < best_length)
            {
              best_i = i;
              best_length = length;
            }
        }
    }
  if (unlikely (best_i == (size_t)(-1)))
    /* Failed to find a gap of sufficient size.  */
    return 0;

  size_t aligned_size = (size + ALIGNMENT - 1) & -ALIGNMENT;

  if (pageptr->common.free_space < aligned_size)
    /* Invalid state: Less free space than expected.  */
    abort ();

  /* Split the gap, leaving an empty gap and a remaining gap.  */
  for (i = num_gaps - 1; ; i--)
    {
      pageptr->gaps[i + 1] = pageptr->gaps[i];
      if (i == best_i)
        break;
    }
  size_t result = pageptr->gaps[best_i].start;
  pageptr->gaps[best_i].end = result;
  pageptr->gaps[best_i + 1].start = result + aligned_size;
  pageptr->num_gaps = num_gaps + 1;
  if (pageptr->num_gaps > PAGESIZE / SMALL_BLOCK_MAX_SIZE + 1)
    /* Invalid state: More gaps than expected.  */
    abort ();

  pageptr->common.free_space -= aligned_size;

  return page + result;
}

static void
free_medium_block_in_page (uintptr_t block, uintptr_t page)
{
  struct medium_page_header *pageptr = (struct medium_page_header *) page;
  size_t offset = block - page;

  /* Search for the gap that ends where this block begins.
     We can ignore the last gap here, since it ends where the page ends.  */
  struct memory_range *gaps = pageptr->gaps;
  size_t lo = 0;
  size_t hi = pageptr->num_gaps - 1;
  size_t index;
  while (lo < hi)
    {
      /* Invariant:
         for i < lo, gaps[i].end < offset,
         for i >= hi, gaps[i].end > offset.  */
      size_t mid = (hi + lo) >> 1; /* >= lo, < hi */
      if (offset > gaps[mid].end)
        lo = mid + 1;
      else if (offset < gaps[mid].end)
        hi = mid;
      else
        {
          /* Found it: offset == gaps[mid].end.  */
          index = mid;
          goto found;
        }
    }
  /* Invalid argument: block is not the start of a currently allocated
     block.  */
  abort ();

 found:
  /* Here 0 <= index < pageptr->num_gaps - 1.
     Combine the gaps index and index+1.  */
  pageptr->common.free_space += gaps[index + 1].start - gaps[index].end;
  if (pageptr->common.free_space < gaps[index + 1].start - gaps[index].end)
    /* Wrap around.  */
    abort ();

  gaps[index].end = gaps[index + 1].end;

  size_t num_gaps = pageptr->num_gaps - 1;
  size_t i;
  for (i = index + 1; i < num_gaps; i++)
    gaps[i] = gaps[i + 1];
  pageptr->num_gaps = num_gaps;
}

/* Management of pages of medium blocks.  */
struct page_pool medium_block_pages =
  {
    init_medium_block_page_pool,
    init_medium_block_page,
    allocate_medium_block_in_page,
    free_medium_block_in_page
  };

/* ==================== Pages of small and medium blocks ==================== */

/* Allocates a block of memory from the given pool, aligned to ALIGNMENT bytes.
   Returns 0 upon failure.  */
static inline uintptr_t
allocate_block_from_pool (size_t size, struct page_pool *pool)
{
  uintptr_t page;

  /* Try in the last used page first.  */
  page = pool->last_page;
  if (likely (page != 0))
    {
      uintptr_t block = pool->allocate_block_in_page (size, page);
      if (likely (block != 0))
        {
          add_update (page, pool);
          return block;
        }
    }

  /* Ensure that the pool and its managed_pages is initialized.  */
  if (unlikely (pool->managed_pages == NULL))
    {
      pool->managed_pages =
        gl_oset_nx_create_empty (GL_RBTREE_OSET, compare_pages_by_free_space, NULL);
      if (unlikely (pool->managed_pages == NULL))
        /* Could not allocate the managed_pages.  */
        return 0;
      pool->init_page_pool (pool);
    }

  /* Ensure that managed_pages is up-to-date.  */
  flush_all_updates (pool);

  /* Try in the other managed_pages.  */
  {
    gl_oset_iterator_t iter =
      gl_oset_iterator_atleast (pool->managed_pages,
                                page_free_space_is_at_least,
                                (void *) (uintptr_t) size);
    const void *elt;
    while (gl_oset_iterator_next (&iter, &elt))
      {
        struct page_tree_element *element = (struct page_tree_element *) elt;
        page = element->page;
        /* No need to try the last used page again.  */
        if (likely (page != pool->last_page))
          {
            uintptr_t block = pool->allocate_block_in_page (size, page);
            if (likely (block != 0))
              {
                gl_oset_iterator_free (&iter);
                add_update (page, pool);
                pool->last_page = page;
                return block;
              }
          }
      }
    gl_oset_iterator_free (&iter);
  }

  /* If we have a freeable page ready for reuse, use it.  */
  if (pool->freeable_page != 0)
    {
      page = pool->freeable_page;
      pool->init_page (page);
      struct page_tree_element *element =
        (struct page_tree_element *) malloc (sizeof (struct page_tree_element));
      if (unlikely (element == NULL))
        {
          /* Could not allocate the tree element.  */
          pool->last_page = 0;
          return 0;
        }
      element->page = page;
      element->free_space = ((struct dissected_page_header *) page)->free_space;
      if (unlikely (gl_oset_nx_add (pool->managed_pages, element) < 0))
        {
          /* Could not allocate the tree node.  */
          free (element);
          pool->last_page = 0;
          return 0;
        }
      ((struct dissected_page_header *) page)->tree_element = element;
      pool->freeable_page = 0;

      uintptr_t block = pool->allocate_block_in_page (size, page);
      if (block == 0)
        /* If the size is too large for an empty page, this function should not
           have been invoked.  */
        abort ();
      add_update (page, pool);
      pool->last_page = page;
      return block;
    }

  /* Allocate a fresh page.  */
  page = ALLOC_PAGES (PAGESIZE);
  if (unlikely (page == 0))
    {
      /* Failed.  */
      pool->last_page = 0;
      return 0;
    }
  if ((page & (PAGESIZE - 1)) != 0)
    /* ALLOC_PAGES's result is not aligned as expected.  */
    abort ();

  pool->init_page (page);
  struct page_tree_element *element =
    (struct page_tree_element *) malloc (sizeof (struct page_tree_element));
  if (unlikely (element == NULL))
    {
      /* Could not allocate the tree element.  */
      FREE_PAGES (page, PAGESIZE);
      pool->last_page = 0;
      return 0;
    }
  element->page = page;
  element->free_space = ((struct dissected_page_header *) page)->free_space;
  if (unlikely (gl_oset_nx_add (pool->managed_pages, element) < 0))
    {
      /* Could not allocate the tree node.  */
      free (element);
      FREE_PAGES (page, PAGESIZE);
      pool->last_page = 0;
      return 0;
    }
  ((struct dissected_page_header *) page)->tree_element = element;

  uintptr_t block = pool->allocate_block_in_page (size, page);
  if (block == 0)
    /* If the size is too large for an empty page, this function should not
       have been invoked.  */
    abort ();
  add_update (page, pool);
  pool->last_page = page;
  return block;
}

static void
free_block_from_pool (uintptr_t block, uintptr_t page, struct page_pool *pool)
{
  if (pool->page_capacity == 0)
    /* Invalid argument: The block is not valid, since the pool has not yet
       been initialized.  */
    abort ();

  pool->free_block_in_page (block, page);

  struct dissected_page_header *pageptr = (struct dissected_page_header *) page;
  if (likely (pageptr->free_space != pool->page_capacity))
    {
      /* The page is not entirely free.  */
      add_update (page, pool);
    }
  else
    {
      /* The page is now entirely free.  */
      /* Remove its tree element and free it.  */
      struct page_tree_element *element = pageptr->tree_element;
      if (!gl_oset_remove (pool->managed_pages, element))
        /* Invalid state: The element is not in the managed_pages.  */
        abort ();
      free (element);

      if (pool->last_page == page)
        pool->last_page = 0;

      drop_update (page, pool);

      /* If we would now have two freeable pages, free the old one.  */
      if (pool->freeable_page != 0)
        FREE_PAGES (pool->freeable_page, PAGESIZE);

      /* Don't free the page now, but later.  */
      pool->freeable_page = page;
    }
}

/* Lock that protects the management of small and medium blocks from
   simultaneous access from multiple threads.  */
gl_lock_define_initialized(static, ssfmalloc_lock)

/* ============================== Large blocks ============================== */

/* Header of a page sequence for a large block.
   Lies at an address that is a multiple of PAGESIZE.  */
struct large_block_header
{
  #if PAGE_RESERVED_HEADER_SIZE > 0
  void * reserved[PAGE_RESERVED_HEADER_SIZE / sizeof (void *)];
  #endif
  unsigned char page_type; /* large_page_type */
};

/* Information about a large block.
   Ends at an address that is a multiple of ALIGNMENT.  */
struct large_block_caption
{
  size_t pages_size; /* A multiple of PAGESIZE.  */
};

/* Size of large block page header, gap, and caption.  */
#define LARGE_BLOCK_OFFSET \
  ((sizeof (struct large_block_header) + sizeof (struct large_block_caption) \
    + ALIGNMENT - 1) & -ALIGNMENT)

/* =========================== Exported functions =========================== */

/* Allocates a block of memory, aligned to ALIGNMENT bytes.
   Returns 0 upon failure.  */
static uintptr_t
allocate_block (size_t size)
{
  uintptr_t block;

  if (unlikely (size > MEDIUM_BLOCKS_PAGE_CAPACITY))
    {
      /* Allocate a large block.  */
      size_t pages_size = (size + LARGE_BLOCK_OFFSET + PAGESIZE - 1) & -PAGESIZE;
      uintptr_t pages = ALLOC_PAGES (pages_size);
      if (unlikely (pages == 0))
        /* Failed.  */
        return 0;
      if ((pages & (PAGESIZE - 1)) != 0)
        /* ALLOC_PAGES's result is not aligned as expected.  */
        abort ();
      ((struct any_page_header *) pages)->page_type = large_page_type;
      block = pages + LARGE_BLOCK_OFFSET;
      ((struct large_block_caption *) block)[-1].pages_size = pages_size;
    }
  else
    {
      bool mt = gl_multithreaded ();
      if (mt) gl_lock_lock (ssfmalloc_lock);
      struct page_pool *pool =
        (size <= SMALL_BLOCK_MAX_SIZE ? &small_block_pages : &medium_block_pages);
      block = allocate_block_from_pool (size, pool);
      if (mt) gl_lock_unlock (ssfmalloc_lock);
    }
  return block;
}

/* Frees a block of memory, returned by allocate_block.  */
static void
free_block (uintptr_t block)
{
  if (block == 0 || (block % ALIGNMENT) != 0)
    /* Invalid argument.  */
    abort ();
  uintptr_t pages = block & -PAGESIZE;
  unsigned char type = ((struct any_page_header *) pages)->page_type;
  if (unlikely (type == large_page_type))
    {
      if (block != pages + LARGE_BLOCK_OFFSET)
        /* Invalid argument.  */
        abort ();
      size_t pages_size = ((struct large_block_caption *) block)[-1].pages_size;
      if ((pages_size & (PAGESIZE - 1)) != 0)
        /* Invalid memory state: pages_size not as expected.  */
        abort ();
      FREE_PAGES (pages, pages_size);
    }
  else
    {
      bool mt = gl_multithreaded ();
      if (mt) gl_lock_lock (ssfmalloc_lock);
      struct page_pool *pool;
      if (type == small_page_type)
        pool = &small_block_pages;
      else if (type == medium_page_type)
        pool = &medium_block_pages;
      else
        /* Invalid memory state: type not as expected.  */
        abort ();
      free_block_from_pool (block, pages, pool);
      if (mt) gl_lock_unlock (ssfmalloc_lock);
    }
}
