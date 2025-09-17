/* Get descriptor for a 32-bit wide character case conversion.
   Copyright (C) 2011-2025 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2023.  */

#include <config.h>

#define IN_C32_GET_MAPPING
/* Specification.  */
#include <uchar.h>

#include <string.h>
#include <wctype.h>

#if _GL_WCHAR_T_IS_UCS4
_GL_EXTERN_INLINE
#endif
c32_mapping_t
c32_get_mapping (const char *name)
{
#if _GL_WCHAR_T_IS_UCS4
  return wctrans (name);
#else
  if (name[0] == 't'
      && name[1] == 'o')
    switch (name[2])
      {
      case 'l':
        if (streq (name + 3, "ower"))
          return c32tolower;
        break;
      case 'u':
        if (streq (name + 3, "pper"))
          return c32toupper;
        break;
      default:
        break;
      }
  return (c32_mapping_t) 0;
#endif
}
