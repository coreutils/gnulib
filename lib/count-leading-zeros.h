/* count-leading-zeros.h -- counts the number of leading 0 bits in a word.
   Copyright (C) 2012 Free Software Foundation, Inc.

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

/* Expand the code which computes the number of leading zeros of the local
   variable 'x' of type TYPE (an unsigned integer type) and returns it
   from the current function.  */
#if 0 && __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4)
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
static inline int
count_leading_zeros_32 (unsigned int x)
{
  int count = 0;
  if (x & 0xffff0000U)
    x >>= 16;
  else
    count += 16;
  if (x & 0xff00)
    x >>= 8;
  else
    count += 8;
  if (x & 0xf0)
    x >>= 4;
  else
    count += 4;
  if (x & 0xc)
    x >>= 2;
  else
    count += 2;
  if (x & 2)
    x >>= 1;
  else
    count++;
  if (!(x & 1))
    count++;
  return count;
}
#endif

/* Compute and return the number of leading zeros in X. */
static inline int
count_leading_zeros (unsigned int x)
{
  COUNT_LEADING_ZEROS (__builtin_clz, unsigned int);
}

/* Compute and return the number of leading zeros in X. */
static inline int
count_leading_zeros_l (unsigned long int x)
{
  COUNT_LEADING_ZEROS (__builtin_clzl, unsigned long int);
}

#if HAVE_UNSIGNED_LONG_LONG_INT
/* Compute and return the number of leading zeros in X. */
static inline int
count_leading_zeros_ll (unsigned long long int x)
{
  COUNT_LEADING_ZEROS (__builtin_clzll, unsigned long long int);
}
#endif

#endif /* COUNT_LEADING_ZEROS_H */
