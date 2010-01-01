/* Check UTF-16 string.
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
#include "unistr.h"

const uint16_t *
u16_check (const uint16_t *s, size_t n)
{
  const uint16_t *s_end = s + n;

  while (s < s_end)
    {
      /* Keep in sync with unistr.h and utf16-ucs4.c.  */
      uint16_t c = *s;

      if (c < 0xd800 || c >= 0xe000)
        {
          s++;
          continue;
        }
      if (c < 0xdc00)
        {
          if (s + 2 <= s_end
              && s[1] >= 0xdc00 && s[1] < 0xe000)
            {
              s += 2;
              continue;
            }
        }
      /* invalid or incomplete multibyte character */
      return s;
    }
  return NULL;
}
