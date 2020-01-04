/* Convert 32-bit wide character to unibyte character.
   Copyright (C) 2020 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Written by Bruno Haible <bruno@clisp.org>, 2020.  */

#include <config.h>

/* Specification.  */
#include <uchar.h>

int
c32tob (wint_t wc)
{
#if _GL_LARGE_CHAR32_T
  /* In all known encodings, unibyte characters correspond only to
     characters in the BMP.  */
  if (wc != WEOF && (wchar_t) wc == wc)
    return wctob ((wchar_t) wc);
  else
    return EOF;
#else
  return wctob (wc);
#endif
}
