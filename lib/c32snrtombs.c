/* Convert 32-bit wide string to string.
   Copyright (C) 2020-2025 Free Software Foundation, Inc.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Written by Bruno Haible <bruno@clisp.org>, 2020.  */

#include <config.h>

#define IN_C32SNRTOMBS
/* Specification.  */
#include <uchar.h>

#include <wchar.h>

#if (HAVE_WORKING_MBRTOC32 && HAVE_WORKING_C32RTOMB && !_GL_WCHAR_T_IS_UCS4) || (GL_CHAR32_T_IS_UNICODE && GL_CHAR32_T_VS_WCHAR_T_NEEDS_CONVERSION) || _GL_SMALL_WCHAR_T
/* The char32_t encoding of a multibyte character may be different than its
   wchar_t encoding, or char32_t is wider than wchar_t.  */

# include <errno.h>
# include <stdlib.h>
# include <string.h>

extern mbstate_t _gl_c32srtombs_state;

# define FUNC c32snrtombs
# define SCHAR_T char32_t
# define INTERNAL_STATE _gl_c32srtombs_state
# define WCRTOMB c32rtomb
# include "wcsnrtombs-impl.h"

#else
/* char32_t and wchar_t are equivalent.  */

static_assert (sizeof (char32_t) == sizeof (wchar_t));

# if _GL_WCHAR_T_IS_UCS4
_GL_EXTERN_INLINE
# endif
size_t
c32snrtombs (char *dest, const char32_t **srcp, size_t srclen, size_t len,
             mbstate_t *ps)
{
  return wcsnrtombs (dest, (const wchar_t **) srcp, srclen, len, ps);
}

#endif
