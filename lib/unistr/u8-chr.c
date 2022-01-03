/* Search character in piece of UTF-8 string.
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

#include <string.h>

uint8_t *
u8_chr (const uint8_t *s, size_t n, ucs4_t uc)
{
  if (uc < 0x80)
    {
      uint8_t c0 = uc;

      return (uint8_t *) memchr ((const char *) s, c0, n);
    }

  {
    uint8_t c[6];
    size_t uc_size;
    uc_size = u8_uctomb_aux (c, uc, 6);

    if (n < uc_size)
      return NULL;

    /* For multibyte character matching we use a Boyer-Moore like
       algorithm that searches for the last byte, skipping multi-byte
       jumps, and matches back from there.

       Instead of using a table as is usual for Boyer-Moore, we compare
       the candidate last byte s[UC_SIZE-1] with each of the possible
       bytes in the UTF-8 representation of UC.  If the final byte does
       not match, we will perform up to UC_SIZE comparisons per memory
       load---but each comparison lets us skip one byte in the input!

       If the final byte matches, the "real" Boyer-Moore algorithm
       is approximated.  Instead, u8_chr just looks for other cN that
       are equal to the final byte and uses those to try realigning to
       another possible match.  For example, when searching for 0xF0
       0xAA 0xBB 0xAA it will always skip forward by two bytes, even if
       the character in the string was for example 0xF1 0xAA 0xBB 0xAA.
       The advantage of this scheme is that the skip count after a failed
       match can be computed outside the loop, and that it keeps the
       complexity low for a pretty rare case.  In particular, since c[0]
       is never between 0x80 and 0xBF, c[0] is never equal to c[UC_SIZE-1]
       and this is optimal for two-byte UTF-8 characters.  */
    switch (uc_size)
      {
      case 2:
        {
          uint8_t c0 = c[0];
          uint8_t c1 = c[1];
          const uint8_t *end = s + n - 1;

          do
            {
              /* Here s < end.
                 Test whether s[0..1] == { c0, c1 }.  */
              uint8_t s1 = s[1];
              if (s1 == c1)
                {
                  if (*s == c0)
                    return (uint8_t *) s;
                  else
                    /* Skip the search at s + 1, because s[1] = c1 < c0.  */
                    s += 2;
                }
              else
                {
                  if (s1 == c0)
                    s++;
                  else
                    /* Skip the search at s + 1, because s[1] != c0.  */
                    s += 2;
                }
            }
          while (s < end);
          break;
        }

      case 3:
        {
          uint8_t c0 = c[0];
          uint8_t c1 = c[1];
          uint8_t c2 = c[2];
          const uint8_t *end = s + n - 2;
          size_t skip;

          if (c2 == c1)
            skip = 1;
          else
            skip = 3;

          do
            {
              /* Here s < end.
                 Test whether s[0..2] == { c0, c1, c2 }.  */
              uint8_t s2 = s[2];
              if (s2 == c2)
                {
                  if (s[1] == c1 && *s == c0)
                    return (uint8_t *) s;
                  else
                    /* If c2 != c1:
                         Skip the search at s + 1, because s[2] == c2 != c1.
                       Skip the search at s + 2, because s[2] == c2 < c0.  */
                    s += skip;
                }
              else
                {
                  if (s2 == c1)
                    s++;
                  else if (s2 == c0)
                    /* Skip the search at s + 1, because s[2] != c1.  */
                    s += 2;
                  else
                    /* Skip the search at s + 1, because s[2] != c1.
                       Skip the search at s + 2, because s[2] != c0.  */
                    s += 3;
                }
            }
          while (s < end);
          break;
        }

      case 4:
        {
          uint8_t c0 = c[0];
          uint8_t c1 = c[1];
          uint8_t c2 = c[2];
          uint8_t c3 = c[3];
          const uint8_t *end = s + n - 3;
          size_t skip;

          if (c3 == c2)
            skip = 1;
          else if (c3 == c1)
            skip = 2;
          else
            skip = 4;

          do
            {
              /* Here s < end.
                 Test whether s[0..3] == { c0, c1, c2, c3 }.  */
              uint8_t s3 = s[3];
              if (s3 == c3)
                {
                  if (s[2] == c2 && s[1] == c1 && *s == c0)
                    return (uint8_t *) s;
                  else
                    /* If c3 != c2:
                         Skip the search at s + 1, because s[3] == c3 != c2.
                       If c3 != c1:
                         Skip the search at s + 2, because s[3] == c3 != c1.
                       Skip the search at s + 3, because s[3] == c3 < c0.  */
                    s += skip;
                }
              else
                {
                  if (s3 == c2)
                    s++;
                  else if (s3 == c1)
                    /* Skip the search at s + 1, because s[3] != c2.  */
                    s += 2;
                  else if (s3 == c0)
                    /* Skip the search at s + 1, because s[3] != c2.
                       Skip the search at s + 2, because s[3] != c1.  */
                    s += 3;
                  else
                    /* Skip the search at s + 1, because s[3] != c2.
                       Skip the search at s + 2, because s[3] != c1.
                       Skip the search at s + 3, because s[3] != c0.  */
                    s += 4;
                }
            }
          while (s < end);
          break;
        }
      }
    return NULL;
  }
}
