/* Apply a 32-bit wide character case conversion.
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

#define IN_C32_APPLY_MAPPING
/* Specification.  */
#include <uchar.h>

#include <string.h>
#include <wctype.h>

#if _GL_WCHAR_T_IS_UCS4
_GL_EXTERN_INLINE
#endif
wint_t
c32_apply_mapping (wint_t wc, c32_mapping_t mapping)
{
#if _GL_WCHAR_T_IS_UCS4
  return towctrans (wc, mapping);
#else
  return mapping (wc);
#endif
}
