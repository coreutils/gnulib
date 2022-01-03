/* Values of numeric Unicode characters.
   Copyright (C) 2002, 2006-2007, 2009-2022 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2002.

   This file is free software.
   It is dual-licensed under "the GNU LGPLv3+ or the GNU GPLv2+".
   You can redistribute it and/or modify it under either
     - the terms of the GNU Lesser General Public License as published
       by the Free Software Foundation, either version 3, or (at your
       option) any later version, or
     - the terms of the GNU General Public License as published by the
       Free Software Foundation; either version 2, or (at your option)
       any later version, or
     - the same dual license "the GNU LGPLv3+ or the GNU GPLv2+".

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License and the GNU General Public License
   for more details.

   You should have received a copy of the GNU Lesser General Public
   License and of the GNU General Public License along with this
   program.  If not, see <https://www.gnu.org/licenses/>.  */

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
              unsigned int index3 = ((uc & numeric_header_4) + lookup2) * 8;
              /* level3 contains 8-bit values, packed into 16-bit words.  */
              unsigned int lookup3 =
                (((unsigned int) u_numeric.level3[index3>>4]
                  | ((unsigned int) u_numeric.level3[(index3>>4)+1] << 16))
                 >> (index3 % 16))
                & 0xff;

              return u_numeric_values[lookup3];
            }
        }
    }
  {
    const uc_fraction_t default_value = { 0, 0 };
    return default_value;
  }
}
