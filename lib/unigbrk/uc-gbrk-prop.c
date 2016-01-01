/* Grapheme cluster break property function.
   Copyright (C) 2010-2016 Free Software Foundation, Inc.
   Written by Ben Pfaff <blp@cs.stanford.edu>, 2010.

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
#include "unigbrk.h"

#include "gbrkprop.h"

int
uc_graphemeclusterbreak_property (ucs4_t uc)
{
  unsigned int index1 = uc >> gbrkprop_header_0;
  if (index1 < gbrkprop_header_1)
    {
      int lookup1 = unigbrkprop.level1[index1];
      if (lookup1 >= 0)
        {
          unsigned int index2 = (uc >> gbrkprop_header_2) & gbrkprop_header_3;
          int lookup2 = unigbrkprop.level2[lookup1 + index2];
          if (lookup2 >= 0)
            {
              unsigned int index3 = uc & gbrkprop_header_4;
              unsigned char lookup3 = unigbrkprop.level3[lookup2 + index3 / 2];
              return (lookup3 >> ((uc & 1) << 2)) & 0x0f;
            }
        }
    }
  return GBP_OTHER;
}
