/* popcount.h -- population count
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

#ifndef POPCOUNT_H
# define POPCOUNT_H 1

#include <limits.h>
#include <stdlib.h>
#include "verify.h"

#if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR >= 4)
#define POPCOUNT_CALCULATION(NAME, TYPE)        \
        return __builtin_##NAME (x);
#else
#define POPCOUNT_CALCULATION(NAME, TYPE)                        \
        int pop = popcount32 (x);                               \
        if (CHAR_BIT * sizeof (TYPE) > 32)                      \
          pop += popcount32 (x >> 31 >> 1);                     \
        (void) verify_true (CHAR_BIT * sizeof (TYPE) <= 64);    \
        return pop;

/* Compute and return the population count of the low 32 bits of
   X, that is, the number of 1-bits set in its least significant
   32 bits. */
static inline int
popcount32 (unsigned int x)
{
  x = ((x & 0xaaaaaaaa) >> 1) + (x & 0x55555555);
  x = ((x & 0xcccccccc) >> 2) + (x & 0x33333333);
  x = ((x & 0xf0f0f0f0) >> 4) + (x & 0x0f0f0f0f);
  x = ((x & 0xff00ff00) >> 8) + (x & 0x00ff00ff);
  return (x >> 16) + (x & 0xff);
}
#endif

/* Compute and return the population count of X, that is, the
   number of 1-bits set in X. */
static inline int
popcount (unsigned int x)
{
  POPCOUNT_CALCULATION (popcount, unsigned int);
}

/* Compute and return the population count of X, that is, the
   number of 1-bits set in X. */
static inline int
popcountl (unsigned long int x)
{
  POPCOUNT_CALCULATION (popcountl, unsigned long int);
}

#if HAVE_UNSIGNED_LONG_LONG_INT
/* Compute and return the population count of X, that is, the
   number of 1-bits set in X. */
static inline int
popcountll (unsigned long long int x)
{
  POPCOUNT_CALCULATION (popcountll, unsigned long long int);
}
#endif

#endif /* POPCOUNT_H */
