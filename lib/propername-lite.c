/* Localization of proper names.
   Copyright 2023-2025 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#include <config.h>

/* Specification.  */
#include "propername.h"

#include "gettext.h"

#if HAVE_UCHAR_H
# include <uchar.h>
/* There is no need for the dependency hassle of replacing glibc mbrtoc32,
   as we don't care whether the C locale treats a byte with the high
   bit set as an encoding error.  */
# ifdef __GLIBC__
#  undef mbrtoc32
# endif
#endif

/* Return the localization of the name spelled NAME_ASCII in ASCII,
   and NAME_UTF8 in UTF-8.  */

char const *
proper_name_lite (char const *name_ascii, _GL_UNUSED char const *name_utf8)
{
  char const *translation = gettext (name_ascii);
  if (translation != name_ascii)
    return translation;

#if HAVE_UCHAR_H
  /* If DF BF decodes to 07FF, assume it is UTF-8.  */
  static char const utf07FF[] = { 0xDF, 0xBF };
  char32_t w;
  mbstate_t mbs; mbszero (&mbs);
  if (mbrtoc32 (&w, utf07FF, 2, &mbs) == 2 && w == 0x07FF)
    return name_utf8;
#endif

  return name_ascii;
}
