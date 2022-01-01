/* Allocate memory with indefinite extent and specified alignment.

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

/* Before including this file, you need to define the following macro:

   ALIGNMENT      A constant expression that evaluates to the desired alignment
                  (a power of 2).

   And you also need to #include <stdint.h> and <stdlib.h>.  */

/* aligned_malloc allocates a block of memory of SIZE bytes, aligned on a
   boundary of ALIGNMENT bytes.
   The block can be freed through aligned_free(), NOT through free().
   Upon failure, it returns NULL.  */

/* This module exists instead of a posix_memalign(), aligned_alloc(), or
   memalign() emulation, because we can't reasonably emulate posix_memalign(),
   aligned_alloc(), or memalign():
   If malloc() returned p, only free (p) is allowed, not free (p + 1),
   free (p + 2), free (p + 4), free (p + 8), or similar.

   We can use posix_memalign(), a POSIX function.

   We can also use aligned_alloc(), an ISO C11 and POSIX function.  But it's
   a bit more awkward to use.

   On older systems, we can alternatively use memalign() instead.  In the
   Solaris documentation of memalign() it is not specified how a memory block
   returned by memalign() can be freed, but it actually can be freed with
   free().  */

#if !defined ALIGNMENT
# error "ALIGNMENT is not defined"
#endif
#if !((ALIGNMENT) > 0 && ((ALIGNMENT) & ((ALIGNMENT) - 1)) == 0)
# error "ALIGNMENT is not a power of 2"
#endif
#if ((ALIGNMENT) <= MALLOC_ALIGNMENT) || HAVE_POSIX_MEMALIGN || HAVE_ALIGNED_ALLOC || HAVE_MEMALIGN

# if defined aligned_free || __GNUC__ >= 11
   /* The caller wants an inline function, not a macro,
      or we can use GCC's -Wmismatched-dealloc warning.  */
static inline void
aligned_free (void *q)
{
  free (q);
}
# else
#  define aligned_free free
# endif

# if (ALIGNMENT) <= MALLOC_ALIGNMENT
/* Simply use malloc.  */

#  if defined aligned_malloc || __GNUC__ >= 11
   /* The caller wants an inline function, not a macro,
      or GCC's -Wmismatched-dealloc warning might be in effect.  */
static inline
/*_GL_ATTRIBUTE_DEALLOC (aligned_free, 1)*/
void *
aligned_malloc (size_t size)
{
  return malloc (size);
}
#  else
#   define aligned_malloc malloc
#  endif

# elif HAVE_POSIX_MEMALIGN
/* Use posix_memalign.
   This is OK since ALIGNMENT > MALLOC_ALIGNMENT >= sizeof (void *).  */

static inline
/*_GL_ATTRIBUTE_DEALLOC (aligned_free, 1)*/
void *
aligned_malloc (size_t size)
{
  void *p;
  int ret = posix_memalign (&p, (ALIGNMENT), size);
  if (ret == 0)
    return p;
  else
    return NULL;
}

# elif HAVE_ALIGNED_ALLOC
/* Use aligned_alloc.  */

static inline
/*_GL_ATTRIBUTE_DEALLOC (aligned_free, 1)*/
void *
aligned_malloc (size_t size)
{
  /* Round up SIZE to the next multiple of ALIGNMENT,
     namely (SIZE + ALIGNMENT - 1) & ~(ALIGNMENT - 1).  */
  size += (ALIGNMENT) - 1;
  if (size >= (ALIGNMENT) - 1) /* no overflow? */
    {
      size &= ~(size_t)((ALIGNMENT) - 1);
      return aligned_alloc ((ALIGNMENT), size);
    }
  return NULL;
}

# elif HAVE_MEMALIGN                    /* HP-UX, IRIX, Solaris <= 10 */
/* Use memalign.  */

static inline
/*_GL_ATTRIBUTE_DEALLOC (aligned_free, 1)*/
void *
aligned_malloc (size_t size)
{
  return memalign ((ALIGNMENT), size);
}

# endif

#else
/* Use malloc and waste a bit of memory.  */

static inline void
aligned_free (void *q)
{
  if (q != NULL)
    {
      if ((uintptr_t) q & ((ALIGNMENT) - 1))
        /* Argument not aligned as expected.  */
        abort ();
      else
        {
          void *p = ((void **) q)[-1];
          if (!((uintptr_t) p <= (uintptr_t) q
                && (uintptr_t) q - (uintptr_t) p >= MALLOC_ALIGNMENT
                && (uintptr_t) q - (uintptr_t) p <= (ALIGNMENT)))
            abort ();
          free (p);
        }
    }
}

static inline
/*_GL_ATTRIBUTE_DEALLOC (aligned_free, 1)*/
void *
aligned_malloc (size_t size)
{
  size += (ALIGNMENT);
  if (size >= (ALIGNMENT)) /* no overflow? */
    {
      void *p = malloc (size);
      if (p != NULL)
        {
          /* Go to the next multiple of ALIGNMENT.  */
          void *q =
            (void *) (((uintptr_t) p + (ALIGNMENT)) & -(intptr_t)(ALIGNMENT));
          /* Now q - p <= ALIGNMENT and
             q - p >= MALLOC_ALIGNMENT >= sizeof (void *).
             This is enough to store a back pointer to p.  */
          ((void **) q)[-1] = p;
          return q;
        }
    }
  return NULL;
}

#endif
