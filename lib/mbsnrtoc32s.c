/* Convert string to 32-bit wide string.
   Copyright (C) 2020-2025 Free Software Foundation, Inc.
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

#define IN_MBSNRTOC32S
/* Specification.  */
#include <uchar.h>

#include <wchar.h>

#if (HAVE_WORKING_MBRTOC32 && HAVE_WORKING_C32RTOMB && !_GL_WCHAR_T_IS_UCS4) || (GL_CHAR32_T_IS_UNICODE && GL_CHAR32_T_VS_WCHAR_T_NEEDS_CONVERSION) || _GL_SMALL_WCHAR_T
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
# if GNULIB_MBRTOC32_REGULAR
   /* If the 'mbrtoc32-regular' module is in use, we don't need to handle
      a (size_t)(-3) return value from mbrtoc32.  */
#  define USES_C32 0
# else
#  define USES_C32 1
# endif
# include "mbsnrtowcs-impl.h"

#else
/* char32_t and wchar_t are equivalent.  */

static_assert (sizeof (char32_t) == sizeof (wchar_t));

# if _GL_WCHAR_T_IS_UCS4
_GL_EXTERN_INLINE
# endif
size_t
mbsnrtoc32s (char32_t *dest, const char **srcp, size_t srclen, size_t len,
             mbstate_t *ps)
{
  return mbsnrtowcs ((wchar_t *) dest, srcp, srclen, len, ps);
}

#endif
