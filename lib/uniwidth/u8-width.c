/* Determine display width of UTF-8 string.
   Copyright (C) 2001-2002, 2006-2007, 2009-2022 Free Software Foundation, Inc.
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
#include "uniwidth.h"

#include "unistr.h"

int
u8_width (const uint8_t *s, size_t n, const char *encoding)
{
  const uint8_t *s_end = s + n;
  int width = 0;

  while (s < s_end)
    {
      ucs4_t uc;
      int w;

      s += u8_mbtouc_unsafe (&uc, s, s_end - s);

      if (uc == 0)
        break; /* end of string reached */

      w = uc_width (uc, encoding);
      if (w >= 0) /* ignore control characters in the string */
        width += w;
    }

  return width;
}
