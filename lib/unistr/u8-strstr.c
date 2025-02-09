/* Substring test for UTF-8 strings.
   Copyright (C) 1999, 2002, 2006, 2010-2025 Free Software Foundation, Inc.
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

/* Don't use the const-improved function macros in this compilation unit.  */
#define _LIBUNISTRING_NO_CONST_GENERICS

#include <config.h>

/* Specification.  */
#include "unistr.h"

#include <string.h>

uint8_t *
u8_strstr (const uint8_t *haystack, const uint8_t *needle)
{
  uint8_t first = needle[0];

  /* Is needle empty?  */
  if (first == 0)
    return (uint8_t *) haystack;

  /* Is needle nearly empty (only one unit)?  */
  if (needle[1] == 0)
    return u8_strchr (haystack, first);

  /* Is needle nearly empty (only one character)?  */
  {
    ucs4_t first_uc;
    int count = u8_strmbtouc (&first_uc, needle);
    if (count > 0 && needle[count] == 0)
      return u8_strchr (haystack, first_uc);
  }

  return (uint8_t *) strstr ((const char *) haystack, (const char *) needle);
}
