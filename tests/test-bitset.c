/* Test of bitset.
   Copyright (C) 2018 Free Software Foundation, Inc.

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

#include "bitset.h"

#include "macros.h"

void check_attributes (enum bitset_attr attr)
{
  enum { nbits = 32 };

  bitset bs0 = bitset_create (nbits, attr);
  ASSERT (bitset_size (bs0) == 32);
  ASSERT (bitset_count (bs0) == 0);
  ASSERT (bitset_empty_p (bs0));

  bitset bs1 = bitset_create (nbits, attr);
  bitset_set (bs1, 1);
  bitset_set (bs1, 3);
  bitset_set (bs1, 5);
  ASSERT (bitset_count (bs1) == 3);
  ASSERT (!bitset_empty_p (bs1));

  bitset bs2 = bitset_create (nbits, attr);
  bitset_set (bs2, 0);
  bitset_set (bs2, 2);
  bitset_set (bs2, 4);

  /* disjoint_p */
  ASSERT (bitset_disjoint_p (bs1, bs2));

  /* and */
  bitset bs = bitset_create (nbits, attr);
  bitset_and (bs, bs1, bs2);
  ASSERT (bitset_count (bs) == 0);

  /* or */
  bitset_or (bs, bs1, bs2);
  ASSERT (bitset_count (bs) == 6);
}

int main (void)
{
  check_attributes (BITSET_FIXED);
  check_attributes (BITSET_VARIABLE);
  check_attributes (BITSET_DENSE);
  check_attributes (BITSET_SPARSE);
  check_attributes (BITSET_FRUGAL);
  check_attributes (BITSET_GREEDY);
  return 0;
}
