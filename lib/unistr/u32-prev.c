/* Iterate over previous character in UTF-32 string.
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
#include "unistr.h"

const uint32_t *
u32_prev (ucs4_t *puc, const uint32_t *s, const uint32_t *start)
{
  if (s != start)
    {
      uint32_t c_1 = s[-1];

      if (c_1 < 0xd800 || (c_1 >= 0xe000 && c_1 < 0x110000))
        {
          *puc = c_1;
          return s - 1;
        }
    }
  return NULL;
}
