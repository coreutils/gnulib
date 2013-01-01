/* count-one-bits.h -- counts the number of 1-bits in a word.
   Copyright (C) 2007-2013 Free Software Foundation, Inc.

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

/* Written by Ben Pfaff.  */

#ifndef COUNT_ONE_BITS_H
# define COUNT_ONE_BITS_H 1

#include <stdlib.h>
#include "verify.h"

_GL_INLINE_HEADER_BEGIN
#ifndef COUNT_ONE_BITS_INLINE
# define COUNT_ONE_BITS_INLINE _GL_INLINE
#endif

/* Expand the code which computes the number of 1-bits of the local
   variable 'x' of type TYPE (an unsigned integer type) and returns it
   from the current function.  */
#if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4)
#define COUNT_ONE_BITS(BUILTIN, TYPE)              \
        return BUILTIN (x);
#else
#define COUNT_ONE_BITS(BUILTIN, TYPE)                                       \
        /* This condition is written so as to avoid shifting by more than   \
           31 bits at once, and also avoids a random HP-UX cc bug.  */      \
        verify (((TYPE) -1 >> 31 >> 31 >> 2) == 0); /* TYPE has at most 64 bits */ \
        int count = count_one_bits_32 (x);                                  \
        if (1 < (TYPE) -1 >> 31) /* TYPE has more than 32 bits? */          \
          count += count_one_bits_32 (x >> 31 >> 1);                        \
        return count;

/* Compute and return the number of 1-bits set in the least
   significant 32 bits of X. */
COUNT_ONE_BITS_INLINE int
count_one_bits_32 (unsigned int x)
{
  x = ((x & 0xaaaaaaaaU) >> 1) + (x & 0x55555555U);
  x = ((x & 0xccccccccU) >> 2) + (x & 0x33333333U);
  x = (x >> 16) + (x & 0xffff);
  x = ((x & 0xf0f0) >> 4) + (x & 0x0f0f);
  return (x >> 8) + (x & 0x00ff);
}
#endif

/* Compute and return the number of 1-bits set in X. */
COUNT_ONE_BITS_INLINE int
count_one_bits (unsigned int x)
{
  COUNT_ONE_BITS (__builtin_popcount, unsigned int);
}

/* Compute and return the number of 1-bits set in X. */
COUNT_ONE_BITS_INLINE int
count_one_bits_l (unsigned long int x)
{
  COUNT_ONE_BITS (__builtin_popcountl, unsigned long int);
}

#if HAVE_UNSIGNED_LONG_LONG_INT
/* Compute and return the number of 1-bits set in X. */
COUNT_ONE_BITS_INLINE int
count_one_bits_ll (unsigned long long int x)
{
  COUNT_ONE_BITS (__builtin_popcountll, unsigned long long int);
}
#endif

_GL_INLINE_HEADER_END

#endif /* COUNT_ONE_BITS_H */
