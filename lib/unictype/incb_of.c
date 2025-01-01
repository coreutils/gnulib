/* Indic_Conjunct_Break attribute of Unicode characters.
   Copyright (C) 2011-2025 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2024.

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

/* Define u_indic_conjunct_break table.  */
#include "incb_of.h"

int
uc_indic_conjunct_break (ucs4_t uc)
{
  unsigned int index1 = uc >> indic_conjunct_break_header_0;
  if (index1 < indic_conjunct_break_header_1)
    {
      int lookup1 = u_indic_conjunct_break.level1[index1];
      if (lookup1 >= 0)
        {
          unsigned int index2 = (uc >> indic_conjunct_break_header_2) & indic_conjunct_break_header_3;
          int lookup2 = u_indic_conjunct_break.level2[lookup1 + index2];
          if (lookup2 >= 0)
            {
              unsigned int index3 = (uc & indic_conjunct_break_header_4) + lookup2;
              /* level3 contains 2-bit values.  */
              unsigned int lookup3 = u_indic_conjunct_break.level3[index3 >> 3];

              return (lookup3 >> (2 * (index3 & 7))) & 3;
            }
        }
    }
  return UC_INDIC_CONJUNCT_BREAK_NONE;
}
