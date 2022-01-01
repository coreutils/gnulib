/* Convert unibyte character to 32-bit wide character.
   Copyright (C) 2020-2022 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2020.  */

#include <config.h>

/* Specification.  */
#include <uchar.h>

#include <stdio.h>
#include <string.h>

wint_t
btoc32 (int c)
{
#if HAVE_WORKING_MBRTOC32 && !defined __GLIBC__
  /* The char32_t encoding of a multibyte character may be different than its
     wchar_t encoding.  */
  if (c != EOF)
    {
      mbstate_t state;
      char s[1];
      char32_t wc;

      memset (&state, '\0', sizeof (mbstate_t));
      s[0] = (unsigned char) c;
      if (mbrtoc32 (&wc, s, 1, &state) <= 1)
        return wc;
    }
  return WEOF;
#else
  /* In all known locale encodings, unibyte characters correspond only to
     characters in the BMP.  */
  return btowc (c);
#endif
}
