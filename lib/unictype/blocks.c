/* Blocks of Unicode characters.
   Copyright (C) 2007, 2009-2010 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2007.

   This program is free software: you can redistribute it and/or modify it
   under the terms of the GNU Lesser General Public License as published
   by the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include <config.h>

/* Specification.  */
#include "unictype.h"

#include "blocks.h"

const uc_block_t *
uc_block (ucs4_t uc)
{
  unsigned int first_index;
  unsigned int last_index;

  if (uc < blocks_level1_threshold)
    {
      unsigned int index1 = uc >> blocks_level1_shift;
      first_index = blocks_level1[2 * index1];
      last_index = blocks_level1[2 * index1 + 1];
    }
  else
    {
      first_index = blocks_upper_first_index;
      last_index = blocks_upper_last_index;
    }
  /* We know that the relevant blocks are blocks[i] with
     first_index <= i < last_index.  Now perform a binary search.  */
  while (first_index < last_index)
    {
      unsigned int mid_index = (first_index + last_index) / 2;
      if (blocks[mid_index].end < uc)
        first_index = mid_index + 1;
      else if (uc < blocks[mid_index].start)
        last_index = mid_index;
      else
        return &blocks[mid_index];
    }
  return NULL;
}

void
uc_all_blocks (const uc_block_t **blocksp, size_t *countp)
{
  *blocksp = blocks;
  *countp = sizeof (blocks) / sizeof (blocks[0]);
}
