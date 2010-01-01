/* Word break property.
   Copyright (C) 2001-2003, 2006-2010 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2009.

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
#include "uniwbrk.h"

#include "wbrkprop.h"

int
uc_wordbreak_property (ucs4_t uc)
{
  unsigned int index1 = uc >> wbrkprop_header_0;
  if (index1 < wbrkprop_header_1)
    {
      int lookup1 = uniwbrkprop.level1[index1];
      if (lookup1 >= 0)
        {
          unsigned int index2 = (uc >> wbrkprop_header_2) & wbrkprop_header_3;
          int lookup2 = uniwbrkprop.level2[lookup1 + index2];
          if (lookup2 >= 0)
            {
              unsigned int index3 = uc & wbrkprop_header_4;
              return uniwbrkprop.level3[lookup2 + index3];
            }
        }
    }
  return WBP_OTHER;
}
