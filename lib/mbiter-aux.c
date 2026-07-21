/* Auxiliary functions for iterating through multibyte strings.
   Copyright (C) 2026 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>.  */

#include <config.h>

/* Specification.  */
#include "mbiter-aux.h"

#include <uchar.h>

bool
mbiter_is_utf8 (int *cache)
{
  if (*cache < 0)
    {
      /* UTF-8 is the only encoding in use which maps the bytes 0xC4 0x80
         to U+0100.  (See libiconv/tests/?*.TXT for all the mapping tables.)
         We can assume that in this case, the char32_t encoding is Unicode
         (not platform-dependent like for other locale encodings).  */
      mbstate_t state; mbszero (&state);
      char32_t wc;
      *cache = (mbrtoc32 (&wc, "\xc4\x80", 2, &state) == 2 && wc == 0x100);
    }
  return *cache;
}

/* If the current locale encoding is UTF-8 and a preceding
     mbrtoc32 (&uc, S, N, &state)
   invocation returned (size_t) -1, this function returns the number of
   initial bytes that form a maximal subpart in the sense of
   https://www.unicode.org/versions/Unicode15.0.0/ch03.pdf page 127..129.
   The result is >= 1, <= N.  */
size_t
mbiter_utf8_maximal_subpart (const char *s, size_t n)
{
  /* Based on lib/unistr/u8-mbtouc.c.  */
  if (n >= 2)
    {
      unsigned char c = (unsigned char) *s;
      if (c >= 0xe0)
        {
          if (c < 0xf0)
            {
              unsigned char c2 = (unsigned char) s[1];
              if ((c2 ^ 0x80) < 0x40
                  && (c >= 0xe1 || c2 >= 0xa0)
                  && (c != 0xed || c2 < 0xa0))
                return 2;
            }
          else if (c <= 0xf4)
            {
              unsigned char c2 = (unsigned char) s[1];
              if ((c2 ^ 0x80) < 0x40
                  && (c >= 0xf1 || c2 >= 0x90)
                  && (c < 0xf4 || (/* c == 0xf4 && */ c2 < 0x90)))
                {
                  if (n >= 3 && ((unsigned char) s[2] ^ 0x80) < 0x40)
                    return 3;
                  else
                    return 2;
                }
            }
        }
    }
  return 1;
}
