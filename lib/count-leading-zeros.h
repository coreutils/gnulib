/* count-leading-zeros.h -- counts the number of leading 0 bits in a word.
   Copyright (C) 2012-2013 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* Written by Eric Blake.  */

#ifndef COUNT_LEADING_ZEROS_H
# define COUNT_LEADING_ZEROS_H 1

#include <limits.h>
#include <stdlib.h>
#include "verify.h"

_GL_INLINE_HEADER_BEGIN
#ifndef COUNT_LEADING_ZEROS_INLINE
# define COUNT_LEADING_ZEROS_INLINE _GL_INLINE
#endif

/* Expand the code which computes the number of leading zeros of the local
   variable 'x' of type TYPE (an unsigned integer type) and returns it
   from the current function.  */
#if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4)
# define COUNT_LEADING_ZEROS(BUILTIN, TYPE)     \
  return x ? BUILTIN (x) : CHAR_BIT * sizeof x;
#else
# define COUNT_LEADING_ZEROS(BUILTIN, TYPE)                             \
  /* This condition is written so as to avoid shifting by more than     \
     31 bits at once, and also avoids a random HP-UX cc bug.  */        \
  verify (((TYPE) -1 >> 31 >> 31 >> 2) == 0); /* TYPE has at most 64 bits */ \
  int count = 0;                                                        \
  if (1 < (TYPE) -1 >> 31) { /* TYPE has more than 32 bits? */          \
    count = count_leading_zeros_32 (x >> 31 >> 1);                      \
    if (count < 32)                                                     \
      return count;                                                     \
  }                                                                     \
  return count + count_leading_zeros_32 (x);

/* Compute and return the number of leading zeros in the least
   significant 32 bits of X. */
COUNT_LEADING_ZEROS_INLINE int
count_leading_zeros_32 (unsigned int x)
{
  /* http://graphics.stanford.edu/~seander/bithacks.html */
  static const char deBruijnLookup[32] = {
    0, 9, 1, 10, 13, 21, 2, 29, 11, 14, 16, 18, 22, 25, 3, 30,
    8, 12, 20, 28, 15, 17, 24, 7, 19, 27, 23, 6, 26, 5, 4, 31
  };

  x &= 0xffffffffU;
  if (!x)
    return 32;
  x |= x >> 1;
  x |= x >> 2;
  x |= x >> 4;
  x |= x >> 8;
  x |= x >> 16;
  return 31 - deBruijnLookup[(x * 0x07c4acddU) >> 27];
}
#endif

/* Compute and return the number of leading zeros in X. */
COUNT_LEADING_ZEROS_INLINE int
count_leading_zeros (unsigned int x)
{
  COUNT_LEADING_ZEROS (__builtin_clz, unsigned int);
}

/* Compute and return the number of leading zeros in X. */
COUNT_LEADING_ZEROS_INLINE int
count_leading_zeros_l (unsigned long int x)
{
  COUNT_LEADING_ZEROS (__builtin_clzl, unsigned long int);
}

#if HAVE_UNSIGNED_LONG_LONG_INT
/* Compute and return the number of leading zeros in X. */
COUNT_LEADING_ZEROS_INLINE int
count_leading_zeros_ll (unsigned long long int x)
{
  COUNT_LEADING_ZEROS (__builtin_clzll, unsigned long long int);
}
#endif

_GL_INLINE_HEADER_END

#endif /* COUNT_LEADING_ZEROS_H */
