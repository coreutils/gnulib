/* xmalloc.c -- malloc with out of memory checking

   Copyright (C) 1990-2000, 2002-2006, 2008-2021 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#include <config.h>

#define XALLOC_INLINE _GL_EXTERN_INLINE

#include "xalloc.h"

#include "intprops.h"
#include "minmax.h"

#include <stdlib.h>
#include <string.h>

/* Allocate N bytes of memory dynamically, with error checking.  */

void *
xmalloc (size_t n)
{
  void *p = malloc (n);
  if (!p)
    xalloc_die ();
  return p;
}

/* Change the size of an allocated block of memory P to N bytes,
   with error checking.  */

void *
xrealloc (void *p, size_t n)
{
  void *r = realloc (p, n);
  if (!r && (!p || n))
    xalloc_die ();
  return r;
}

/* Change the size of an allocated block of memory P to an array of N
   objects each of S bytes, with error checking.  */

void *
xreallocarray (void *p, size_t n, size_t s)
{
  void *r = reallocarray (p, n, s);
  if (!r && (!p || (n && s)))
    xalloc_die ();
  return r;
}

/* If P is null, allocate a block of at least *PN bytes; otherwise,
   reallocate P so that it contains more than *PN bytes.  *PN must be
   nonzero unless P is null.  Set *PN to the new block's size, and
   return the pointer to the new block.  *PN is never set to zero, and
   the returned pointer is never null.  */

void *
x2realloc (void *p, size_t *pn)
{
  return x2nrealloc (p, pn, 1);
}

/* Grow PA, which points to an array of *NITEMS items, and return the
   location of the reallocated array, updating *NITEMS to reflect its
   new size.  The new array will contain at least NITEMS_INCR_MIN more
   items, but will not contain more than NITEMS_MAX items total.
   ITEM_SIZE is the size of each item, in bytes.

   ITEM_SIZE and NITEMS_INCR_MIN must be positive.  *NITEMS must be
   nonnegative.  If NITEMS_MAX is -1, it is treated as if it were
   infinity.

   If PA is null, then allocate a new array instead of reallocating
   the old one.

   Thus, to grow an array A without saving its old contents, do
   { free (A); A = xpalloc (NULL, &AITEMS, ...); }.  */

void *
xpalloc (void *pa, idx_t *nitems, idx_t nitems_incr_min,
         ptrdiff_t nitems_max, idx_t item_size)
{
  idx_t n0 = *nitems;

  /* The approximate size to use for initial small allocation
     requests.  This is the largest "small" request for the GNU C
     library malloc.  */
  enum { DEFAULT_MXFAST = 64 * sizeof (size_t) / 4 };

  /* If the array is tiny, grow it to about (but no greater than)
     DEFAULT_MXFAST bytes.  Otherwise, grow it by about 50%.
     Adjust the growth according to three constraints: NITEMS_INCR_MIN,
     NITEMS_MAX, and what the C language can represent safely.  */

  idx_t n;
  if (INT_ADD_WRAPV (n0, n0 >> 1, &n))
    n = IDX_MAX;
  if (0 <= nitems_max && nitems_max < n)
    n = nitems_max;

  /* NBYTES is of a type suitable for holding the count of bytes in an object.
     This is typically idx_t, but it should be size_t on (theoretical?)
     platforms where SIZE_MAX < IDX_MAX so xpalloc does not pass
     values greater than SIZE_MAX to xrealloc.  */
#if IDX_MAX <= SIZE_MAX
  idx_t nbytes;
#else
  size_t nbytes;
#endif
  idx_t adjusted_nbytes
    = (INT_MULTIPLY_WRAPV (n, item_size, &nbytes)
       ? MIN (IDX_MAX, SIZE_MAX)
       : nbytes < DEFAULT_MXFAST ? DEFAULT_MXFAST : 0);
  if (adjusted_nbytes)
    {
      n = adjusted_nbytes / item_size;
      nbytes = adjusted_nbytes - adjusted_nbytes % item_size;
    }

  if (! pa)
    *nitems = 0;
  if (n - n0 < nitems_incr_min
      && (INT_ADD_WRAPV (n0, nitems_incr_min, &n)
          || (0 <= nitems_max && nitems_max < n)
          || INT_MULTIPLY_WRAPV (n, item_size, &nbytes)))
    xalloc_die ();
  pa = xrealloc (pa, nbytes);
  *nitems = n;
  return pa;
}

/* Allocate N bytes of zeroed memory dynamically, with error checking.
   There's no need for xnzalloc (N, S), since it would be equivalent
   to xcalloc (N, S).  */

void *
xzalloc (size_t n)
{
  return xcalloc (n, 1);
}

/* Allocate zeroed memory for N elements of S bytes, with error
   checking.  S must be nonzero.  */

void *
xcalloc (size_t n, size_t s)
{
  void *p = calloc (n, s);
  if (!p)
    xalloc_die ();
  return p;
}

/* Clone an object P of size S, with error checking.  There's no need
   for xnmemdup (P, N, S), since xmemdup (P, N * S) works without any
   need for an arithmetic overflow check.  */

void *
xmemdup (void const *p, size_t s)
{
  return memcpy (xmalloc (s), p, s);
}

/* Clone STRING.  */

char *
xstrdup (char const *string)
{
  return xmemdup (string, strlen (string) + 1);
}
