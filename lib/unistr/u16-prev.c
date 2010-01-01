/* Iterate over previous character in UTF-16 string.
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
u16_prev (ucs4_t *puc, const uint16_t *s, const uint16_t *start)
{
  /* Keep in sync with unistr.h and utf16-ucs4.c.  */
  if (s != start)
    {
      uint16_t c_1 = s[-1];

      if (c_1 < 0xd800 || c_1 >= 0xe000)
        {
          *puc = c_1;
          return s - 1;
        }
#if CONFIG_UNICODE_SAFETY
      if (c_1 >= 0xdc00)
#endif
        if (s - 1 != start)
          {
            uint16_t c_2 = s[-2];

#if CONFIG_UNICODE_SAFETY
            if (c_2 >= 0xd800 && c_2 < 0xdc00)
#endif
              {
                *puc = 0x10000 + ((c_2 - 0xd800) << 10) + (c_1 - 0xdc00);
                return s - 2;
              }
          }
    }
  return NULL;
}
