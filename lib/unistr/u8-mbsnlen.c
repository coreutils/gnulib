/* Count characters in UTF-8 string.
   Copyright (C) 2007, 2009-2022 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2007.

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

size_t
u8_mbsnlen (const uint8_t *s, size_t n)
{
  size_t characters;

  characters = 0;
  while (n > 0)
    {
      ucs4_t uc;
      int count = u8_mbtoucr (&uc, s, n);
      characters++;
      if (count == -2)
        break;
      if (count < 0)
        count = u8_mbtouc (&uc, s, n);
      else if (count == 0)
        count = 1;
      s += count;
      n -= count;
    }
  return characters;
}
