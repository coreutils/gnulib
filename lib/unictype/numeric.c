/* Values of numeric Unicode characters.
   Copyright (C) 2002, 2006-2007, 2009-2010 Free Software Foundation, Inc.
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

/* Define u_numeric_values and u_numeric tables.  */
#include "numeric.h"

uc_fraction_t
uc_numeric_value (ucs4_t uc)
{
  unsigned int index1 = uc >> numeric_header_0;
  if (index1 < numeric_header_1)
    {
      int lookup1 = u_numeric.level1[index1];
      if (lookup1 >= 0)
        {
          unsigned int index2 = (uc >> numeric_header_2) & numeric_header_3;
          int lookup2 = u_numeric.level2[lookup1 + index2];
          if (lookup2 >= 0)
            {
              unsigned int index3 = ((uc & numeric_header_4) + lookup2) * 7;
              /* level3 contains 7-bit values, packed into 16-bit words.  */
              unsigned int lookup3 =
                ((u_numeric.level3[index3>>4]
                  | (u_numeric.level3[(index3>>4)+1] << 16))
                 >> (index3 % 16))
                & 0x7f;

              return u_numeric_values[lookup3];
            }
        }
    }
  {
    const uc_fraction_t default_value = { 0, 0 };
    return default_value;
  }
}
