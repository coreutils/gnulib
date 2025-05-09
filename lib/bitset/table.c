/* Functions to support expandable bitsets.

   Copyright (C) 2002-2006, 2009-2015, 2018-2025 Free Software Foundation, Inc.

   Contributed by Michael Hayes (m.hayes@elec.canterbury.ac.nz).

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#include <config.h>

#include "bitset/table.h"

#include <stdlib.h>
#include <string.h>

#include "obstack.h"
#include "xalloc.h"

/* This file implements expandable bitsets.  These bitsets can be of
   arbitrary length and are more efficient than arrays of bits for
   large sparse sets.

   Empty elements are represented by a NULL pointer in the table of
   element pointers.  An alternative is to point to a special zero
   element.  Similarly, we could represent an all 1's element with
   another special ones element pointer.

   Bitsets are commonly empty so we need to ensure that this special
   case is fast.  A zero bitset is indicated when cdata is 0.  This is
   conservative since cdata may be non zero and the bitset may still
   be zero.

   The bitset cache can be disabled either by setting cindex to
   BITSET_WINDEX_MAX or by setting csize to 0.  Here
   we use the former approach since cindex needs to be updated whenever
   cdata is changed.
*/


/* Number of words to use for each element.  */
#define TBITSET_ELT_WORDS 2

/* Number of bits stored in each element.  */
#define TBITSET_ELT_BITS \
  ((unsigned) (TBITSET_ELT_WORDS * BITSET_WORD_BITS))

/* Tbitset element.  We use an array of bits.  */
typedef struct tbitset_elt_struct
{
  union
  {
    bitset_word words[TBITSET_ELT_WORDS];       /* Bits that are set.  */
    struct tbitset_elt_struct *next;
  }
  u;
}
tbitset_elt;


typedef tbitset_elt *tbitset_elts;


/* Number of elements to initially allocate.  */

#ifndef TBITSET_INITIAL_SIZE
# define TBITSET_INITIAL_SIZE 2
#endif


enum tbitset_find_mode
  { TBITSET_FIND, TBITSET_CREATE, TBITSET_SUBST };

static tbitset_elt tbitset_zero_elts[1]; /* Elements of all zero bits.  */

/* Obstack to allocate bitset elements from.  */
static struct obstack tbitset_obstack;
static bool tbitset_obstack_init = false;
static tbitset_elt *tbitset_free_list;  /* Free list of bitset elements.  */

#define TBITSET_N_ELTS(N) (((N) + TBITSET_ELT_BITS - 1) / TBITSET_ELT_BITS)
#define TBITSET_ELTS(BSET) ((BSET)->e.elts)
#define TBITSET_SIZE(BSET) TBITSET_N_ELTS (BITSET_NBITS_ (BSET))
#define TBITSET_ASIZE(BSET) ((BSET)->e.size)

#define TBITSET_NEXT(ELT) ((ELT)->u.next)
#define TBITSET_WORDS(ELT) ((ELT)->u.words)

/* Disable bitset cache and mark BSET as being zero.  */
#define TBITSET_ZERO_SET(BSET) ((BSET)->b.cindex = BITSET_WINDEX_MAX, \
        (BSET)->b.cdata = NULL)

#define TBITSET_CACHE_DISABLE(BSET)  ((BSET)->b.cindex = BITSET_WINDEX_MAX)

/* Disable bitset cache and mark BSET as being possibly non-zero.  */
#define TBITSET_NONZERO_SET(BSET) \
 (TBITSET_CACHE_DISABLE (BSET), (BSET)->b.cdata = (bitset_word *)~0)

/* A conservative estimate of whether the bitset is zero.
   This is non-zero only if we know for sure that the bitset is zero.  */
#define TBITSET_ZERO_P(BSET) ((BSET)->b.cdata == NULL)

/* Enable cache to point to element with table index EINDEX.
   The element must exist.  */
#define TBITSET_CACHE_SET(BSET, EINDEX) \
 ((BSET)->b.cindex = (EINDEX) * TBITSET_ELT_WORDS, \
  (BSET)->b.cdata = TBITSET_WORDS (TBITSET_ELTS (BSET) [EINDEX]))

#undef min
#undef max
#define min(a, b) ((a) > (b) ? (b) : (a))
#define max(a, b) ((a) > (b) ? (a) : (b))

static bitset_bindex
tbitset_resize (bitset src, bitset_bindex n_bits)
{
  if (n_bits == BITSET_NBITS_ (src))
    return n_bits;

  bitset_windex oldsize = TBITSET_SIZE (src);
  bitset_windex newsize = TBITSET_N_ELTS (n_bits);

  if (oldsize < newsize)
    {
      /* The bitset needs to grow.  If we already have enough memory
         allocated, then just zero what we need.  */
      if (newsize > TBITSET_ASIZE (src))
        {
          /* We need to allocate more memory.  When oldsize is
             non-zero this means that we are changing the size, so
             grow the bitset 25% larger than requested to reduce
             number of reallocations.  */

          bitset_windex size = oldsize == 0 ? newsize : newsize + newsize / 4;
          TBITSET_ELTS (src)
            = xrealloc (TBITSET_ELTS (src), size * sizeof (tbitset_elt *));
          TBITSET_ASIZE (src) = size;
        }

      memset (TBITSET_ELTS (src) + oldsize, 0,
              (newsize - oldsize) * sizeof (tbitset_elt *));
    }
  else
    {
      /* The bitset needs to shrink.  There's no point deallocating
         the memory unless it is shrinking by a reasonable amount.  */
      if ((oldsize - newsize) >= oldsize / 2)
        {
          void *p
            = realloc (TBITSET_ELTS (src), newsize * sizeof (tbitset_elt *));
          if (p)
            {
              TBITSET_ELTS (src) = p;
              TBITSET_ASIZE (src) = newsize;
            }
        }

      /* Need to prune any excess bits.  FIXME.  */
    }

  BITSET_NBITS_ (src) = n_bits;
  return n_bits;
}


/* Allocate a tbitset element.  The bits are not cleared.  */
static inline tbitset_elt *
tbitset_elt_alloc (void)
{
  tbitset_elt *elt;

  if (tbitset_free_list != NULL)
    {
      elt = tbitset_free_list;
      tbitset_free_list = TBITSET_NEXT (elt);
    }
  else
    {
      if (!tbitset_obstack_init)
        {
          tbitset_obstack_init = true;

          /* Let particular systems override the size of a chunk.  */

#ifndef OBSTACK_CHUNK_SIZE
# define OBSTACK_CHUNK_SIZE 0
#endif

          /* Let them override the alloc and free routines too.  */

#ifndef OBSTACK_CHUNK_ALLOC
# define OBSTACK_CHUNK_ALLOC xmalloc
#endif

#ifndef OBSTACK_CHUNK_FREE
# define OBSTACK_CHUNK_FREE free
#endif

#if !(defined __GNUC__ || defined __clang__)
# define __alignof__(type) 0
#endif

          obstack_specify_allocation (&tbitset_obstack, OBSTACK_CHUNK_SIZE,
                                      __alignof__ (tbitset_elt),
                                      OBSTACK_CHUNK_ALLOC,
                                      OBSTACK_CHUNK_FREE);
        }

      /* Perhaps we should add a number of new elements to the free
         list.  */
      elt = (tbitset_elt *) obstack_alloc (&tbitset_obstack,
                                           sizeof (tbitset_elt));
    }

  return elt;
}


/* Allocate a tbitset element.  The bits are cleared.  */
static inline tbitset_elt *
tbitset_elt_calloc (void)
{
  tbitset_elt *elt = tbitset_elt_alloc ();
  memset (TBITSET_WORDS (elt), 0, sizeof (TBITSET_WORDS (elt)));
  return elt;
}


static inline void
tbitset_elt_free (tbitset_elt *elt)
{
  TBITSET_NEXT (elt) = tbitset_free_list;
  tbitset_free_list = elt;
}


/* Remove element with index EINDEX from bitset BSET.  */
static inline void
tbitset_elt_remove (bitset bset, bitset_windex eindex)
{
  tbitset_elts *elts = TBITSET_ELTS (bset);
  tbitset_elt *elt = elts[eindex];

  elts[eindex] = NULL;
  tbitset_elt_free (elt);
}


/* Add ELT into elts at index EINDEX of bitset BSET.  */
static inline void
tbitset_elt_add (bitset bset, tbitset_elt *elt, bitset_windex eindex)
{
  tbitset_elts *elts = TBITSET_ELTS (bset);
  /* Assume that the elts entry not allocated.  */
  elts[eindex] = elt;
}


/* Are all bits in an element zero?  */
static inline bool
tbitset_elt_zero_p (tbitset_elt *elt)
{
  for (int i = 0; i < TBITSET_ELT_WORDS; i++)
    if (TBITSET_WORDS (elt)[i])
      return false;
  return true;
}


static tbitset_elt *
tbitset_elt_find (bitset bset, bitset_bindex bindex,
                  enum tbitset_find_mode mode)
{
  bitset_windex eindex = bindex / TBITSET_ELT_BITS;

  tbitset_elts *elts = TBITSET_ELTS (bset);
  bitset_windex size = TBITSET_SIZE (bset);

  if (eindex < size)
    {
      tbitset_elt *elt = elts[eindex];
      if (elt)
        {
          if (TBITSET_WORDS (elt) != bset->b.cdata)
            TBITSET_CACHE_SET (bset, eindex);
          return elt;
        }
    }

  /* The element could not be found.  */

  switch (mode)
    {
    default:
      abort ();

    case TBITSET_FIND:
      return NULL;

    case TBITSET_CREATE:
      if (eindex >= size)
        tbitset_resize (bset, bindex);

      /* Create a new element.  */
      {
        tbitset_elt *elt = tbitset_elt_calloc ();
        tbitset_elt_add (bset, elt, eindex);
        TBITSET_CACHE_SET (bset, eindex);
        return elt;
      }

    case TBITSET_SUBST:
      return &tbitset_zero_elts[0];
    }
}


/* Weed out the zero elements from the elts.  */
static inline bitset_windex
tbitset_weed (bitset bset)
{
  if (TBITSET_ZERO_P (bset))
    return 0;

  tbitset_elts *elts = TBITSET_ELTS (bset);
  bitset_windex count = 0;
  bitset_windex j;
  for (j = 0; j < TBITSET_SIZE (bset); j++)
    {
      tbitset_elt *elt = elts[j];

      if (elt)
        {
          if (tbitset_elt_zero_p (elt))
            {
              tbitset_elt_remove (bset, j);
              count++;
            }
        }
      else
        count++;
    }

  count = j - count;
  if (!count)
    {
      /* All the bits are zero.  We could shrink the elts.
         For now just mark BSET as known to be zero.  */
      TBITSET_ZERO_SET (bset);
    }
  else
    TBITSET_NONZERO_SET (bset);

  return count;
}


/* Set all bits in the bitset to zero.  */
static inline void
tbitset_zero (bitset bset)
{
  if (TBITSET_ZERO_P (bset))
    return;

  tbitset_elts *elts = TBITSET_ELTS (bset);
  for (bitset_windex j = 0; j < TBITSET_SIZE (bset); j++)
    {
      tbitset_elt *elt = elts[j];
      if (elt)
        tbitset_elt_remove (bset, j);
    }

  /* All the bits are zero.  We could shrink the elts.
     For now just mark BSET as known to be zero.  */
  TBITSET_ZERO_SET (bset);
}


static inline bool
tbitset_equal_p (bitset dst, bitset src)
{
  if (src == dst)
    return true;

  tbitset_weed (dst);
  tbitset_weed (src);

  if (TBITSET_SIZE (src) != TBITSET_SIZE (dst))
    return false;

  tbitset_elts *selts = TBITSET_ELTS (src);
  tbitset_elts *delts = TBITSET_ELTS (dst);

  for (bitset_windex j = 0; j < TBITSET_SIZE (src); j++)
    {
      tbitset_elt *selt = selts[j];
      tbitset_elt *delt = delts[j];

      if (!selt && !delt)
        continue;
      if ((selt && !delt) || (!selt && delt))
        return false;

      for (unsigned i = 0; i < TBITSET_ELT_WORDS; i++)
        if (TBITSET_WORDS (selt)[i] != TBITSET_WORDS (delt)[i])
          return false;
    }
  return true;
}


/* Copy bits from bitset SRC to bitset DST.  */
static inline void
tbitset_copy_ (bitset dst, bitset src)
{
  if (src == dst)
    return;

  tbitset_zero (dst);

  if (BITSET_NBITS_ (dst) != BITSET_NBITS_ (src))
    tbitset_resize (dst, BITSET_NBITS_ (src));

  tbitset_elts *selts = TBITSET_ELTS (src);
  tbitset_elts *delts = TBITSET_ELTS (dst);
  for (bitset_windex j = 0; j < TBITSET_SIZE (src); j++)
    {
      tbitset_elt *selt = selts[j];
      if (selt)
        {
          tbitset_elt *tmp = tbitset_elt_alloc ();
          delts[j] = tmp;
          memcpy (TBITSET_WORDS (tmp), TBITSET_WORDS (selt),
                  sizeof (TBITSET_WORDS (selt)));
        }
    }
  TBITSET_NONZERO_SET (dst);
}


/* Copy bits from bitset SRC to bitset DST.  Return true if
   bitsets different.  */
static inline bool
tbitset_copy_cmp (bitset dst, bitset src)
{
  if (src == dst)
    return false;

  if (TBITSET_ZERO_P (dst))
    {
      tbitset_copy_ (dst, src);
      return !TBITSET_ZERO_P (src);
    }

  if (tbitset_equal_p (dst, src))
    return false;

  tbitset_copy_ (dst, src);
  return true;
}


/* Set bit BITNO in bitset DST.  */
static void
tbitset_set (bitset dst, bitset_bindex bitno)
{
  bitset_windex windex = bitno / BITSET_WORD_BITS;

  tbitset_elt_find (dst, bitno, TBITSET_CREATE);

  dst->b.cdata[windex - dst->b.cindex] |=
    (bitset_word) 1 << (bitno % BITSET_WORD_BITS);
}


/* Reset bit BITNO in bitset DST.  */
static void
tbitset_reset (bitset dst, bitset_bindex bitno)
{
  bitset_windex windex = bitno / BITSET_WORD_BITS;

  if (!tbitset_elt_find (dst, bitno, TBITSET_FIND))
    return;

  dst->b.cdata[windex - dst->b.cindex] &=
    ~((bitset_word) 1 << (bitno % BITSET_WORD_BITS));

  /* If all the data is zero, perhaps we should remove it now...
     However, there is a good chance that the element will be needed
     again soon.  */
}


/* Test bit BITNO in bitset SRC.  */
static bool
tbitset_test (bitset src, bitset_bindex bitno)
{
  bitset_windex windex = bitno / BITSET_WORD_BITS;

  return (tbitset_elt_find (src, bitno, TBITSET_FIND)
          && ((src->b.cdata[windex - src->b.cindex]
               >> (bitno % BITSET_WORD_BITS))
              & 1));
}


static void
tbitset_free (bitset bset)
{
  tbitset_zero (bset);
  free (TBITSET_ELTS (bset));
}


/* Find list of up to NUM bits set in BSET starting from and including
 *NEXT and store in array LIST.  Return with actual number of bits
 found and with *NEXT indicating where search stopped.  */
static bitset_bindex
tbitset_list_reverse (bitset bset, bitset_bindex *list,
                      bitset_bindex num, bitset_bindex *next)
{
  if (TBITSET_ZERO_P (bset))
    return 0;

  bitset_windex size = TBITSET_SIZE (bset);
  bitset_bindex n_bits = size * TBITSET_ELT_BITS;
  bitset_bindex rbitno = *next;

  if (rbitno >= n_bits)
    return 0;

  tbitset_elts *elts = TBITSET_ELTS (bset);

  bitset_bindex bitno = n_bits - (rbitno + 1);

  bitset_windex windex = bitno / BITSET_WORD_BITS;
  bitset_windex eindex = bitno / TBITSET_ELT_BITS;
  bitset_windex woffset = windex - eindex * TBITSET_ELT_WORDS;

  /* If num is 1, we could speed things up with a binary search
     of the word of interest.  */
  bitset_bindex count = 0;
  unsigned bitcnt = bitno % BITSET_WORD_BITS;
  bitset_bindex bitoff = windex * BITSET_WORD_BITS;

  do
    {
      tbitset_elt *elt = elts[eindex];
      if (elt)
        {
          bitset_word *srcp = TBITSET_WORDS (elt);

          do
            {
              bitset_word word = srcp[woffset];
              if (bitcnt + 1 < BITSET_WORD_BITS)
                /* We're starting in the middle of a word: smash bits to ignore.  */
                word &= ((bitset_word) 1 << (bitcnt + 1)) - 1;
              BITSET_FOR_EACH_BIT_REVERSE(pos, word)
                {
                  list[count++] = bitoff + pos;
                  if (count >= num)
                    {
                      *next = n_bits - (bitoff + pos);
                      return count;
                    }
                }
              bitoff -= BITSET_WORD_BITS;
              bitcnt = BITSET_WORD_BITS - 1;
            }
          while (woffset--);
        }

      woffset = TBITSET_ELT_WORDS - 1;
      bitoff = eindex * TBITSET_ELT_BITS - BITSET_WORD_BITS;
    }
  while (eindex--);

  *next = n_bits - (bitoff + 1);
  return count;
}


/* Find list of up to NUM bits set in BSET starting from and including
   *NEXT and store in array LIST.  Return with actual number of bits
   found and with *NEXT indicating where search stopped.  */
static bitset_bindex
tbitset_list (bitset bset, bitset_bindex *list,
              bitset_bindex num, bitset_bindex *next)
{
  if (TBITSET_ZERO_P (bset))
    return 0;

  bitset_bindex bitno = *next;
  bitset_bindex count = 0;

  tbitset_elts *elts = TBITSET_ELTS (bset);
  bitset_windex size = TBITSET_SIZE (bset);
  bitset_windex eindex = bitno / TBITSET_ELT_BITS;

  if (bitno % TBITSET_ELT_BITS)
    {
      /* We need to start within an element.  This is not very common.  */
      tbitset_elt *elt = elts[eindex];
      if (elt)
        {
          bitset_word *srcp = TBITSET_WORDS (elt);
          bitset_windex woffset = eindex * TBITSET_ELT_WORDS;

          for (bitset_windex windex = bitno / BITSET_WORD_BITS;
               (windex - woffset) < TBITSET_ELT_WORDS; windex++)
            {
              bitset_word word = srcp[windex - woffset] >> (bitno % BITSET_WORD_BITS);

              BITSET_FOR_EACH_BIT (pos, word)
                {
                  list[count++] = bitno + pos;
                  if (count >= num)
                    {
                      *next = bitno + pos + 1;
                      return count;
                    }
                }
              bitno = (windex + 1) * BITSET_WORD_BITS;
            }
        }

      /* Skip to next element.  */
      eindex++;
    }

  /* If num is 1, we could speed things up with a binary search
     of the word of interest.  */

  for (; eindex < size; eindex++)
    {
      tbitset_elt *elt = elts[eindex];
      if (!elt)
        continue;

      bitset_word *srcp = TBITSET_WORDS (elt);
      bitset_windex windex = eindex * TBITSET_ELT_WORDS;
      bitno = windex * BITSET_WORD_BITS;

      /* Is there enough room to avoid checking in each iteration? */
      if ((count + TBITSET_ELT_BITS) < num)
        {
          /* The coast is clear, plant boot!  */

#if TBITSET_ELT_WORDS == 2
          bitset_word word = srcp[0];
          if (word)
            BITSET_FOR_EACH_BIT (pos, word)
              list[count++] = bitno + pos;
          windex++;
          bitno = windex * BITSET_WORD_BITS;

          word = srcp[1];
          if (word)
            BITSET_FOR_EACH_BIT (pos, word)
              list[count++] = bitno + pos;
          windex++;
          bitno = windex * BITSET_WORD_BITS;
#else
          for (int i = 0; i < TBITSET_ELT_WORDS; i++, windex++)
            {
              bitset_word word = srcp[i];
              if (word)
                BITSET_FOR_EACH_BIT (pos, word)
                  list[count++] = bitno + pos;
              bitno = windex * BITSET_WORD_BITS;
            }
#endif
        }
      else
        {
          /* Tread more carefully since we need to check
             if array overflows.  */
          for (int i = 0; i < TBITSET_ELT_WORDS; i++)
            {
              bitset_word word = srcp[i];
              if (word)
                BITSET_FOR_EACH_BIT (pos, word)
                  {
                    list[count++] = bitno + pos;
                    if (count >= num)
                      {
                        *next = bitno + pos + 1;
                        return count;
                      }
                  }
              windex++;
              bitno = windex * BITSET_WORD_BITS;
            }
        }
    }

  *next = bitno;
  return count;
}


/* Ensure that any unused bits within the last element are clear.  */
static inline void
tbitset_unused_clear (bitset dst)
{
  bitset_bindex n_bits = BITSET_NBITS_ (dst);
  unsigned last_bit = n_bits % TBITSET_ELT_BITS;

  if (last_bit)
    {
      tbitset_elts *elts = TBITSET_ELTS (dst);

      bitset_windex eindex = n_bits / TBITSET_ELT_BITS;

      tbitset_elt *elt = elts[eindex];
      if (elt)
        {
          bitset_word *srcp = TBITSET_WORDS (elt);

          bitset_windex windex = n_bits / BITSET_WORD_BITS;
          bitset_windex woffset = eindex * TBITSET_ELT_WORDS;

          srcp[windex - woffset]
            &= ((bitset_word) 1 << (last_bit % BITSET_WORD_BITS)) - 1;
          windex++;
          for (; (windex - woffset) < TBITSET_ELT_WORDS; windex++)
            srcp[windex - woffset] = 0;
        }
    }
}


static void
tbitset_ones (bitset dst)
{
  for (bitset_windex j = 0; j < TBITSET_SIZE (dst); j++)
    {
      /* Create new elements if they cannot be found.  Perhaps
         we should just add pointers to a ones element?  */
      tbitset_elt *elt =
        tbitset_elt_find (dst, j * TBITSET_ELT_BITS, TBITSET_CREATE);
      memset (TBITSET_WORDS (elt), -1, sizeof (TBITSET_WORDS (elt)));
    }
  TBITSET_NONZERO_SET (dst);
  tbitset_unused_clear (dst);
}


static bool
tbitset_empty_p (bitset dst)
{
  if (TBITSET_ZERO_P (dst))
    return true;

  tbitset_elts *elts = TBITSET_ELTS (dst);
  for (bitset_windex j = 0; j < TBITSET_SIZE (dst); j++)
    {
      tbitset_elt *elt = elts[j];

      if (elt)
        {
          if (!tbitset_elt_zero_p (elt))
            return false;
          /* Do some weeding as we go.  */
          tbitset_elt_remove (dst, j);
        }
    }

  /* All the bits are zero.  We could shrink the elts.
     For now just mark DST as known to be zero.  */
  TBITSET_ZERO_SET (dst);
  return true;
}


static void
tbitset_not (bitset dst, bitset src)
{
  tbitset_resize (dst, BITSET_NBITS_ (src));

  for (bitset_windex j = 0; j < TBITSET_SIZE (src); j++)
    {
      /* Create new elements for dst if they cannot be found
         or substitute zero elements if src elements not found.  */
      tbitset_elt *selt =
        tbitset_elt_find (src, j * TBITSET_ELT_BITS, TBITSET_SUBST);
      tbitset_elt *delt =
        tbitset_elt_find (dst, j * TBITSET_ELT_BITS, TBITSET_CREATE);

      for (unsigned i = 0; i < TBITSET_ELT_WORDS; i++)
        TBITSET_WORDS (delt)[i] = ~TBITSET_WORDS (selt)[i];
    }
  TBITSET_NONZERO_SET (dst);
  tbitset_unused_clear (dst);
}


/* Is DST == DST | SRC?  */
static bool
tbitset_subset_p (bitset dst, bitset src)
{
  tbitset_elts *selts = TBITSET_ELTS (src);
  tbitset_elts *delts = TBITSET_ELTS (dst);

  bitset_windex ssize = TBITSET_SIZE (src);
  bitset_windex dsize = TBITSET_SIZE (dst);

  for (bitset_windex j = 0; j < ssize; j++)
    {
      tbitset_elt *selt = j < ssize ? selts[j] : NULL;
      tbitset_elt *delt = j < dsize ? delts[j] : NULL;

      if (!selt && !delt)
        continue;

      if (!selt)
        selt = &tbitset_zero_elts[0];
      if (!delt)
        delt = &tbitset_zero_elts[0];

      for (unsigned i = 0; i < TBITSET_ELT_WORDS; i++)
        if (TBITSET_WORDS (delt)[i]
            != (TBITSET_WORDS (selt)[i] | TBITSET_WORDS (delt)[i]))
          return false;
    }
  return true;
}


/* Is DST & SRC == 0?  */
static bool
tbitset_disjoint_p (bitset dst, bitset src)
{
  tbitset_elts *selts = TBITSET_ELTS (src);
  tbitset_elts *delts = TBITSET_ELTS (dst);

  bitset_windex ssize = TBITSET_SIZE (src);
  bitset_windex dsize = TBITSET_SIZE (dst);

  for (bitset_windex j = 0; j < ssize; j++)
    {
      tbitset_elt *selt = j < ssize ? selts[j] : NULL;
      tbitset_elt *delt = j < dsize ? delts[j] : NULL;

      if (!selt || !delt)
        continue;

      for (unsigned i = 0; i < TBITSET_ELT_WORDS; i++)
        if ((TBITSET_WORDS (selt)[i] & TBITSET_WORDS (delt)[i]))
          return false;
    }
  return true;
}



static bool
tbitset_op3_cmp (bitset dst, bitset src1, bitset src2, enum bitset_ops op)
{
  bool changed = false;

  tbitset_resize (dst, max (BITSET_NBITS_ (src1), BITSET_NBITS_ (src2)));

  bitset_windex ssize1 = TBITSET_SIZE (src1);
  bitset_windex ssize2 = TBITSET_SIZE (src2);
  bitset_windex dsize = TBITSET_SIZE (dst);
  bitset_windex size = ssize1;
  if (size < ssize2)
    size = ssize2;

  tbitset_elts *selts1 = TBITSET_ELTS (src1);
  tbitset_elts *selts2 = TBITSET_ELTS (src2);
  tbitset_elts *delts = TBITSET_ELTS (dst);

  bitset_windex j = 0;
  for (j = 0; j < size; j++)
    {
      tbitset_elt *selt1 = j < ssize1 ? selts1[j] : NULL;
      tbitset_elt *selt2 = j < ssize2 ? selts2[j] : NULL;
      tbitset_elt *delt = j < dsize ? delts[j] : NULL;

      if (!selt1 && !selt2)
        {
          if (delt)
            {
              changed = true;
              tbitset_elt_remove (dst, j);
            }
          continue;
        }

      if (!selt1)
        selt1 = &tbitset_zero_elts[0];
      if (!selt2)
        selt2 = &tbitset_zero_elts[0];
      if (!delt)
        delt = tbitset_elt_calloc ();
      else
        delts[j] = NULL;

      bitset_word *srcp1 = TBITSET_WORDS (selt1);
      bitset_word *srcp2 = TBITSET_WORDS (selt2);
      bitset_word *dstp = TBITSET_WORDS (delt);
      switch (op)
        {
        default:
          abort ();

        case BITSET_OP_OR:
          for (unsigned i = 0; i < TBITSET_ELT_WORDS; i++, dstp++)
            {
              bitset_word tmp = *srcp1++ | *srcp2++;

              if (*dstp != tmp)
                {
                  changed = true;
                  *dstp = tmp;
                }
            }
          break;

        case BITSET_OP_AND:
          for (unsigned i = 0; i < TBITSET_ELT_WORDS; i++, dstp++)
            {
              bitset_word tmp = *srcp1++ & *srcp2++;

              if (*dstp != tmp)
                {
                  changed = true;
                  *dstp = tmp;
                }
            }
          break;

        case BITSET_OP_XOR:
          for (unsigned i = 0; i < TBITSET_ELT_WORDS; i++, dstp++)
            {
              bitset_word tmp = *srcp1++ ^ *srcp2++;

              if (*dstp != tmp)
                {
                  changed = true;
                  *dstp = tmp;
                }
            }
          break;

        case BITSET_OP_ANDN:
          for (unsigned i = 0; i < TBITSET_ELT_WORDS; i++, dstp++)
            {
              bitset_word tmp = *srcp1++ & ~(*srcp2++);

              if (*dstp != tmp)
                {
                  changed = true;
                  *dstp = tmp;
                }
            }
          break;
        }

      if (!tbitset_elt_zero_p (delt))
        tbitset_elt_add (dst, delt, j);
      else
        tbitset_elt_free (delt);
    }

  /* If we have elements of DST left over, free them all.  */
  for (; j < dsize; j++)
    {
      changed = true;

      tbitset_elt *delt = delts[j];
      if (delt)
        tbitset_elt_remove (dst, j);
    }

  TBITSET_NONZERO_SET (dst);
  return changed;
}


static bool
tbitset_and_cmp (bitset dst, bitset src1, bitset src2)
{
  if (TBITSET_ZERO_P (src2))
    {
      tbitset_weed (dst);
      bool changed = TBITSET_ZERO_P (dst);
      tbitset_zero (dst);
      return changed;
    }
  else if (TBITSET_ZERO_P (src1))
    {
      tbitset_weed (dst);
      bool changed = TBITSET_ZERO_P (dst);
      tbitset_zero (dst);
      return changed;
    }
  else
    return tbitset_op3_cmp (dst, src1, src2, BITSET_OP_AND);
}


static void
tbitset_and (bitset dst, bitset src1, bitset src2)
{
  tbitset_and_cmp (dst, src1, src2);
}


static bool
tbitset_andn_cmp (bitset dst, bitset src1, bitset src2)
{
  if (TBITSET_ZERO_P (src2))
    return tbitset_copy_cmp (dst, src1);
  else if (TBITSET_ZERO_P (src1))
    {
      tbitset_weed (dst);
      bool changed = TBITSET_ZERO_P (dst);
      tbitset_zero (dst);
      return changed;
    }
  else
    return tbitset_op3_cmp (dst, src1, src2, BITSET_OP_ANDN);
}


static void
tbitset_andn (bitset dst, bitset src1, bitset src2)
{
  tbitset_andn_cmp (dst, src1, src2);
}


static bool
tbitset_or_cmp (bitset dst, bitset src1, bitset src2)
{
  if (TBITSET_ZERO_P (src2))
    return tbitset_copy_cmp (dst, src1);
  else if (TBITSET_ZERO_P (src1))
    return tbitset_copy_cmp (dst, src2);
  else
    return tbitset_op3_cmp (dst, src1, src2, BITSET_OP_OR);
}


static void
tbitset_or (bitset dst, bitset src1, bitset src2)
{
  tbitset_or_cmp (dst, src1, src2);
}


static bool
tbitset_xor_cmp (bitset dst, bitset src1, bitset src2)
{
  if (TBITSET_ZERO_P (src2))
    return tbitset_copy_cmp (dst, src1);
  else if (TBITSET_ZERO_P (src1))
    return tbitset_copy_cmp (dst, src2);
  else
    return tbitset_op3_cmp (dst, src1, src2, BITSET_OP_XOR);
}


static void
tbitset_xor (bitset dst, bitset src1, bitset src2)
{
  tbitset_xor_cmp (dst, src1, src2);
}


static void
tbitset_copy (bitset dst, bitset src)
{
  if (BITSET_COMPATIBLE_ (dst, src))
    tbitset_copy_ (dst, src);
  else
    bitset_copy_ (dst, src);
}


/* Vector of operations for linked-list bitsets.  */
static struct bitset_vtable tbitset_vtable = {
  tbitset_set,
  tbitset_reset,
  bitset_toggle_,
  tbitset_test,
  tbitset_resize,
  bitset_size_,
  bitset_count_,
  tbitset_empty_p,
  tbitset_ones,
  tbitset_zero,
  tbitset_copy,
  tbitset_disjoint_p,
  tbitset_equal_p,
  tbitset_not,
  tbitset_subset_p,
  tbitset_and,
  tbitset_and_cmp,
  tbitset_andn,
  tbitset_andn_cmp,
  tbitset_or,
  tbitset_or_cmp,
  tbitset_xor,
  tbitset_xor_cmp,
  bitset_and_or_,
  bitset_and_or_cmp_,
  bitset_andn_or_,
  bitset_andn_or_cmp_,
  bitset_or_and_,
  bitset_or_and_cmp_,
  tbitset_list,
  tbitset_list_reverse,
  tbitset_free,
  BITSET_TABLE
};


/* Return size of initial structure.  */
size_t
tbitset_bytes (MAYBE_UNUSED bitset_bindex n_bits)
{
  return sizeof (struct tbitset_struct);
}


/* Initialize a bitset.  */

bitset
tbitset_init (bitset bset, bitset_bindex n_bits)
{
  bset->b.vtable = &tbitset_vtable;

  bset->b.csize = TBITSET_ELT_WORDS;

  TBITSET_ZERO_SET (bset);

  TBITSET_ASIZE (bset) = 0;
  TBITSET_ELTS (bset) = NULL;
  tbitset_resize (bset, n_bits);

  return bset;
}


void
tbitset_release_memory (void)
{
  tbitset_free_list = NULL;
  if (tbitset_obstack_init)
    {
      tbitset_obstack_init = false;
      obstack_free (&tbitset_obstack, NULL);
    }
}
