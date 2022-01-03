/* Convert string to 32-bit wide string.
   Copyright (C) 2020-2022 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2020.

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

#include <config.h>

/* Specification.  */
#include <uchar.h>

#include <wchar.h>

#if (HAVE_WORKING_MBRTOC32 && !defined __GLIBC__) || _GL_LARGE_CHAR32_T
/* The char32_t encoding of a multibyte character may be different than its
   wchar_t encoding, or char32_t is wider than wchar_t.  */

/* For Cygwin >= 1.7 it would be possible to speed this up a bit by cutting
   the source into chunks, calling mbsnrtowcs on a chunk, then u16_to_u32 on
   the result, then proceed with the next chunk, and so on.
   But speed is not critical here so far.  */

/* Reuse the implementation of mbsnrtowcs with a different parameterization.  */

# include <errno.h>
# include <limits.h>
# include <stdlib.h>

# include "minmax.h"
# include "strnlen1.h"

extern mbstate_t _gl_mbsrtoc32s_state;

# define FUNC mbsnrtoc32s
# define DCHAR_T char32_t
# define INTERNAL_STATE _gl_mbsrtoc32s_state
# define MBRTOWC mbrtoc32
# include "mbsnrtowcs-impl.h"

#else
/* char32_t and wchar_t are equivalent.  */

# include "verify.h"

verify (sizeof (char32_t) == sizeof (wchar_t));

size_t
mbsnrtoc32s (char32_t *dest, const char **srcp, size_t srclen, size_t len,
             mbstate_t *ps)
{
  return mbsnrtowcs ((wchar_t *) dest, srcp, srclen, len, ps);
}

#endif
