/* Convert wide string to string.
   Copyright (C) 2008, 2010-2022 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2008.

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
#include <wchar.h>

extern mbstate_t _gl_wcsrtombs_state;

#if HAVE_WCSRTOMBS && !WCSRTOMBS_TERMINATION_BUG && !defined GNULIB_defined_mbstate_t
/* Override the system's wcsrtombs() function.  */

# undef wcsrtombs

size_t
rpl_wcsrtombs (char *dest, const wchar_t **srcp, size_t len, mbstate_t *ps)
{
  if (ps == NULL)
    ps = &_gl_wcsrtombs_state;
# if WCSRTOMBS_NULL_ARG_BUG
  if (dest == NULL)
    {
      const wchar_t *temp_src = *srcp;

      return wcsrtombs (NULL, &temp_src, (size_t)-1, ps);
    }
  else
# endif
    return wcsrtombs (dest, srcp, len, ps);
}

#else
/* Implement wcsrtombs on top of wcrtomb().  */

# include <errno.h>
# include <stdlib.h>
# include <string.h>

# define FUNC wcsrtombs
# define SCHAR_T wchar_t
# define INTERNAL_STATE _gl_wcsrtombs_state
# define WCRTOMB wcrtomb
# include "wcsrtombs-impl.h"

#endif
