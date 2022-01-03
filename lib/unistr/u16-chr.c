/* Search character in piece of UTF-16 string.
   Copyright (C) 1999, 2002, 2006-2007, 2009-2022 Free Software Foundation,
   Inc.
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
#include "unistr.h"

uint16_t *
u16_chr (const uint16_t *s, size_t n, ucs4_t uc)
{
  uint16_t c[2];

  if (uc < 0x10000)
    {
      uint16_t c0 = uc;

      for (; n > 0; s++, n--)
        {
          if (*s == c0)
            return (uint16_t *) s;
        }
    }
  else
    switch (u16_uctomb_aux (c, uc, 2))
      {
      case 2:
        if (n > 1)
          {
            uint16_t c0 = c[0];
            uint16_t c1 = c[1];

            for (n--; n > 0; s++, n--)
              {
                if (*s == c0 && s[1] == c1)
                  return (uint16_t *) s;
              }
          }
        break;
      }
  return NULL;
}
