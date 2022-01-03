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

uint8_t *
u8_strrchr (const uint8_t *s, ucs4_t uc)
{
  /* Calling u8_strlen and then searching from the other end would cause more
     memory accesses. Avoid that, at the cost of a few more comparisons.  */
  uint8_t *result = NULL;
  uint8_t c[6];

  if (uc < 0x80)
    {
      uint8_t c0 = uc;

      for (;; s++)
        {
          if (*s == c0)
            result = (uint8_t *) s;
          if (*s == 0)
            break;
        }
    }
  else
    switch (u8_uctomb_aux (c, uc, 6))
      {
      case 2:
        if (*s)
          {
            uint8_t c0 = c[0];
            uint8_t c1 = c[1];

            /* FIXME: Maybe walking the string via u8_mblen is a win?  */
            for (;; s++)
              {
                if (s[1] == 0)
                  break;
                if (*s == c0 && s[1] == c1)
                  result = (uint8_t *) s;
              }
          }
        break;

      case 3:
        if (*s && s[1])
          {
            uint8_t c0 = c[0];
            uint8_t c1 = c[1];
            uint8_t c2 = c[2];

            /* FIXME: Maybe walking the string via u8_mblen is a win?  */
            for (;; s++)
              {
                if (s[2] == 0)
                  break;
                if (*s == c0 && s[1] == c1 && s[2] == c2)
                  result = (uint8_t *) s;
              }
          }
        break;

      case 4:
        if (*s && s[1] && s[2])
          {
            uint8_t c0 = c[0];
            uint8_t c1 = c[1];
            uint8_t c2 = c[2];
            uint8_t c3 = c[3];

            /* FIXME: Maybe walking the string via u8_mblen is a win?  */
            for (;; s++)
              {
                if (s[3] == 0)
                  break;
                if (*s == c0 && s[1] == c1 && s[2] == c2 && s[3] == c3)
                  result = (uint8_t *) s;
              }
          }
        break;
      }
  return result;
}
