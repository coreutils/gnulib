/* Values of digit Unicode characters.
   Copyright (C) 2002, 2006, 2009-2010 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2002.

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

/* Define u_digit table.  */
#include "digit.h"

int
uc_digit_value (ucs4_t uc)
{
  unsigned int index1 = uc >> digit_header_0;
  if (index1 < digit_header_1)
    {
      int lookup1 = u_digit.level1[index1];
      if (lookup1 >= 0)
        {
          unsigned int index2 = (uc >> digit_header_2) & digit_header_3;
          int lookup2 = u_digit.level2[lookup1 + index2];
          if (lookup2 >= 0)
            {
              unsigned int index3 = (uc & digit_header_4) + lookup2;
              /* level3 contains 4-bit values.  */
              unsigned int lookup3 =
                (u_digit.level3[index3>>1] >> ((index3 % 2) * 4)) & 0x0f;

              return (int) lookup3 - 1;
            }
        }
    }
  return -1;
}
