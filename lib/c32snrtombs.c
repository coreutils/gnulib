/* Convert 32-bit wide string to string.
   Copyright (C) 2020-2022 Free Software Foundation, Inc.

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

/* Specification.  */
#include <uchar.h>

#include <wchar.h>

#if (HAVE_WORKING_MBRTOC32 && !defined __GLIBC__) || _GL_LARGE_CHAR32_T
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

# include "verify.h"

verify (sizeof (char32_t) == sizeof (wchar_t));

size_t
c32snrtombs (char *dest, const char32_t **srcp, size_t srclen, size_t len,
             mbstate_t *ps)
{
  return wcsnrtombs (dest, (const wchar_t **) srcp, srclen, len, ps);
}

#endif
