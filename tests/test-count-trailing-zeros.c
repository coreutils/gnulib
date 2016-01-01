/* Test counting of trailing zeros.
   Copyright (C) 2013-2016 Free Software Foundation, Inc.

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

/* Written by Paul Eggert.  */

#include <config.h>

#include "count-trailing-zeros.h"

#include <limits.h>
#include <stdio.h>

#include "macros.h"

#define UINT_BIT (sizeof (unsigned int) * CHAR_BIT)
#define ULONG_BIT (sizeof (unsigned long int) * CHAR_BIT)
#define ULLONG_BIT (sizeof (unsigned long long int) * CHAR_BIT)

#ifndef ULLONG_MAX
# define HALF (1ULL << (sizeof (unsigned long long int) * CHAR_BIT - 1))
# define ULLONG_MAX (HALF - 1 + HALF)
#endif

int
main (int argc, char *argv[])
{
  int i, j;

#define TEST_COUNT_TRAILING_ZEROS(FUNC, TYPE, BITS, MAX, ONE)   \
  ASSERT (FUNC (0) == BITS);                                    \
  for (i = 0; i < BITS; i++)                                    \
    {                                                           \
      ASSERT (FUNC (ONE << i) == i);                            \
      for (j = 0; j < i; j++)                                   \
        ASSERT (FUNC ((ONE << i) | (ONE << j)) == j);           \
    }                                                           \
  for (i = 0; i < 1000; i++)                                    \
    {                                                           \
      TYPE value = rand () ^ (rand () << 31 << 1);              \
      int count = 0;                                            \
      for (j = BITS - 1; 0 <= j; j--)                           \
        if (value & (ONE << j))                                 \
          count = j;                                            \
      ASSERT (count == FUNC (value));                           \
    }                                                           \
  ASSERT (FUNC (MAX) == 0);

  TEST_COUNT_TRAILING_ZEROS (count_trailing_zeros, unsigned int,
                             UINT_BIT, UINT_MAX, 1U);
  TEST_COUNT_TRAILING_ZEROS (count_trailing_zeros_l, unsigned long int,
                             ULONG_BIT, ULONG_MAX, 1UL);
#ifdef HAVE_UNSIGNED_LONG_LONG_INT
  TEST_COUNT_TRAILING_ZEROS (count_trailing_zeros_ll, unsigned long long int,
                             ULLONG_BIT, ULLONG_MAX, 1ULL);
#endif

  return 0;
}
