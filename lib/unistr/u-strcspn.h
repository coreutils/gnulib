/* Search for some characters in UTF-8/UTF-16/UTF-32 string.
   Copyright (C) 1999, 2002, 2006, 2009-2022 Free Software Foundation, Inc.
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

size_t
FUNC (const UNIT *str, const UNIT *reject)
{
  /* Optimize two cases.  */
  if (reject[0] == 0)
    return U_STRLEN (str);
  {
    ucs4_t uc;
    int count = U_STRMBTOUC (&uc, reject);
    if (count >= 0 && reject[count] == 0)
      {
        const UNIT *found = U_STRCHR (str, uc);
        if (found != NULL)
          return found - str;
        else
          return U_STRLEN (str);
      }
  }
  /* General case.  */
  {
    const UNIT *ptr = str;

    for (;;)
      {
        ucs4_t uc;
        int count = U_STRMBTOUC (&uc, ptr);
        if (count == 0)
          return ptr - str;
        if (count < 0)
          break;
        if (U_STRCHR (reject, uc))
          return ptr - str;
        ptr += count;
      }
    return U_STRLEN (str);
  }
}
