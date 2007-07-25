/* count-one-bits.h -- counts the number of 1-bits in a word.
   Copyright (C) 2007 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

/* Written by Ben Pfaff.  */

#ifndef COUNT_ONE_BITS_H
# define COUNT_ONE_BITS_H 1

#include <limits.h>
#include <stdlib.h>
#include "verify.h"

#if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR >= 4)
#define COUNT_ONE_BITS(BUILTIN, TYPE)              \
        return BUILTIN (x);
#else
#define COUNT_ONE_BITS(BUILTIN, TYPE)                           \
        int count = count_one_bits_32 (x);                      \
        if (CHAR_BIT * sizeof (TYPE) > 32)                      \
          count += count_one_bits_32 (x >> 31 >> 1);            \
        (void) verify_true (CHAR_BIT * sizeof (TYPE) <= 64);    \
        return count;

/* Compute and return the the number of 1-bits set in the least
   significant 32 bits of X. */
static inline int
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
static inline int
count_one_bits (unsigned int x)
{
  COUNT_ONE_BITS (__builtin_popcount, unsigned int);
}

/* Compute and return the number of 1-bits set in X. */
static inline int
count_one_bits_l (unsigned long int x)
{
  COUNT_ONE_BITS (__builtin_popcountl, unsigned long int);
}

#if HAVE_UNSIGNED_LONG_LONG_INT
/* Compute and return the number of 1-bits set in X. */
static inline int
count_one_bits_ll (unsigned long long int x)
{
  COUNT_ONE_BITS (__builtin_popcountll, unsigned long long int);
}
#endif

#endif /* COUNT_ONE_BITS_H */
