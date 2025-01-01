/* Determine number of screen columns needed for a size-bounded 32-bit wide string.
   Copyright (C) 2023-2025 Free Software Foundation, Inc.

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

#include <config.h>

#define IN_C32SWIDTH
/* Specification.  */
#include <uchar.h>

#if _GL_WCHAR_T_IS_UCS4 && !GNULIB_defined_mbstate_t

# include <wchar.h>

_GL_EXTERN_INLINE
int
c32swidth (const char32_t *s, size_t n)
{
  return wcswidth ((const wchar_t *) s, n);
}

#else

# include <limits.h>

# define FUNC c32swidth
# define UNIT char32_t
# define CHARACTER_WIDTH c32width
# include "wcswidth-impl.h"

#endif
