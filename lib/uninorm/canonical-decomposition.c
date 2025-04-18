/* Canonical decomposition of Unicode characters.
   Copyright (C) 2009-2025 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2009.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of the
   License, or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#include <config.h>

/* Specification.  */
#include "uninorm.h"

#include <stdlib.h>

#include "uninorm/decomposition-table.h"

int
uc_canonical_decomposition (ucs4_t uc, ucs4_t *decomposition)
{
  if (uc >= 0xAC00 && uc < 0xD7A4)
    {
      /* Hangul syllable.  See Unicode standard, chapter 3, section
         "Hangul Syllable Decomposition",  See also the clarification at
         <https://www.unicode.org/versions/Unicode5.1.0/>, section
         "Clarification of Hangul Jamo Handling".  */
      unsigned int t;

      uc -= 0xAC00;
      t = uc % 28;

      if (t == 0)
        {
          unsigned int v, l;

          uc = uc / 28;
          v = uc % 21;
          l = uc / 21;

          decomposition[0] = 0x1100 + l;
          decomposition[1] = 0x1161 + v;
          return 2;
        }
      else
        {
#if 1 /* Return the pairwise decomposition, not the full decomposition.  */
          decomposition[0] = 0xAC00 + uc - t; /* = 0xAC00 + (l * 21 + v) * 28; */
          decomposition[1] = 0x11A7 + t;
          return 2;
#else
          unsigned int v, l;

          uc = uc / 28;
          v = uc % 21;
          l = uc / 21;

          decomposition[0] = 0x1100 + l;
          decomposition[1] = 0x1161 + v;
          decomposition[2] = 0x11A7 + t;
          return 3;
#endif
        }
    }
  else if (uc < 0x110000)
    {
      unsigned short entry = decomp_index (uc);
      /* An entry of (unsigned short)(-1) denotes an absent entry.
         Otherwise, bit 15 of the entry tells whether the decomposition
         is a canonical one.  */
      if (entry < 0x8000)
        {
          const unsigned char *p;
          unsigned int element;
          unsigned int length;

          p = &gl_uninorm_decomp_chars_table[3 * entry];
          element = (p[0] << 16) | (p[1] << 8) | p[2];
          /* The first element has 5 bits for the decomposition type.  */
          if (((element >> 18) & 0x1f) != UC_DECOMP_CANONICAL)
            abort ();
          length = 1;
          for (;;)
            {
              /* Every element has an 18 bits wide Unicode code point.  */
              *decomposition = element & 0x3ffff;
              /* Bit 23 tells whether there are more elements,  */
              if ((element & (1 << 23)) == 0)
                break;
              p += 3;
              element = (p[0] << 16) | (p[1] << 8) | p[2];
              decomposition++;
              length++;
            }
          return length;
        }
    }
  return -1;
}
