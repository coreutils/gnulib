/* Search character in UTF-8 string.
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
u8_strchr (const uint8_t *s, ucs4_t uc)
{
  uint8_t c[6];

  if (uc < 0x80)
    {
      uint8_t c0 = uc;

      if (false)
        {
          /* Unoptimized code.  */
          for (;;)
            {
              uint8_t s0 = *s;
              if (s0 == c0)
                return (uint8_t *) s;
              s++;
              if (s0 == 0)
                break;
            }
        }
      else
        {
          /* Optimized code.
             strchr() is often so well optimized, that it's worth the
             added function call.  */
          return (uint8_t *) strchr ((const char *) s, c0);
        }
    }
  else
      /* Loops equivalent to strstr, optimized for a specific length (2, 3, 4)
         of the needle.  We use an algorithm similar to Boyer-Moore which
         is documented in lib/unistr/u8-chr.c.  There is additional
         complication because we need to check after every byte for
         a NUL byte, but the idea is the same. */
    switch (u8_uctomb_aux (c, uc, 6))
      {
      case 2:
        if (*s == 0 || s[1] == 0)
          break;
        {
          uint8_t c0 = c[0];
          uint8_t c1 = c[1];
          /* Search for { c0, c1 }.  */
          uint8_t s1 = s[1];

          for (;;)
            {
              /* Here s[0] != 0, s[1] != 0.
                 Test whether s[0..1] == { c0, c1 }.  */
              if (s1 == c1)
                {
                  if (*s == c0)
                    return (uint8_t *) s;
                  else
                    /* Skip the search at s + 1, because s[1] = c1 < c0.  */
                    goto case2_skip2;
                }
              else
                {
                  if (s1 == c0)
                    goto case2_skip1;
                  else
                    /* Skip the search at s + 1, because s[1] != c0.  */
                    goto case2_skip2;
                }
             case2_skip2:
              s++;
              s1 = s[1];
              if (s[1] == 0)
                break;
             case2_skip1:
              s++;
              s1 = s[1];
              if (s[1] == 0)
                break;
            }
        }
        break;

      case 3:
        if (*s == 0 || s[1] == 0 || s[2] == 0)
          break;
        {
          uint8_t c0 = c[0];
          uint8_t c1 = c[1];
          uint8_t c2 = c[2];
          /* Search for { c0, c1, c2 }.  */
          uint8_t s2 = s[2];

          for (;;)
            {
              /* Here s[0] != 0, s[1] != 0, s[2] != 0.
                 Test whether s[0..2] == { c0, c1, c2 }.  */
              if (s2 == c2)
                {
                  if (s[1] == c1 && *s == c0)
                    return (uint8_t *) s;
                  else
                    /* If c2 != c1:
                         Skip the search at s + 1, because s[2] == c2 != c1.
                       Skip the search at s + 2, because s[2] == c2 < c0.  */
                    if (c2 == c1)
                      goto case3_skip1;
                    else
                      goto case3_skip3;
                }
              else
                {
                  if (s2 == c1)
                    goto case3_skip1;
                  else if (s2 == c0)
                    /* Skip the search at s + 1, because s[2] != c1.  */
                    goto case3_skip2;
                  else
                    /* Skip the search at s + 1, because s[2] != c1.
                       Skip the search at s + 2, because s[2] != c0.  */
                    goto case3_skip3;
                }
             case3_skip3:
              s++;
              s2 = s[2];
              if (s[2] == 0)
                break;
             case3_skip2:
              s++;
              s2 = s[2];
              if (s[2] == 0)
                break;
             case3_skip1:
              s++;
              s2 = s[2];
              if (s[2] == 0)
                break;
            }
        }
        break;

      case 4:
        if (*s == 0 || s[1] == 0 || s[2] == 0 || s[3] == 0)
          break;
        {
          uint8_t c0 = c[0];
          uint8_t c1 = c[1];
          uint8_t c2 = c[2];
          uint8_t c3 = c[3];
          /* Search for { c0, c1, c2, c3 }.  */
          uint8_t s3 = s[3];

          for (;;)
            {
              /* Here s[0] != 0, s[1] != 0, s[2] != 0, s[3] != 0.
                 Test whether s[0..3] == { c0, c1, c2, c3 }.  */
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
                    if (c3 == c2)
                      goto case4_skip1;
                    else if (c3 == c1)
                      goto case4_skip2;
                    else
                      goto case4_skip4;
                }
              else
                {
                  if (s3 == c2)
                    goto case4_skip1;
                  else if (s3 == c1)
                    /* Skip the search at s + 1, because s[3] != c2.  */
                    goto case4_skip2;
                  else if (s3 == c0)
                    /* Skip the search at s + 1, because s[3] != c2.
                       Skip the search at s + 2, because s[3] != c1.  */
                    goto case4_skip3;
                  else
                    /* Skip the search at s + 1, because s[3] != c2.
                       Skip the search at s + 2, because s[3] != c1.
                       Skip the search at s + 3, because s[3] != c0.  */
                    goto case4_skip4;
                }
             case4_skip4:
              s++;
              s3 = s[3];
              if (s[3] == 0)
                break;
             case4_skip3:
              s++;
              s3 = s[3];
              if (s[3] == 0)
                break;
             case4_skip2:
              s++;
              s3 = s[3];
              if (s[3] == 0)
                break;
             case4_skip1:
              s++;
              s3 = s[3];
              if (s[3] == 0)
                break;
            }
        }
        break;
      }

  return NULL;
}
