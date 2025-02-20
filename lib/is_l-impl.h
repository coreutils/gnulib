/* Test whether a single-byte character belongs to a specific character class.
   Copyright (C) 2025 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2025.  */

#include <locale.h>
#include <stdio.h>

int
FUNC (int c, locale_t locale)
{
  struct gl_locale_category_t *plc =
    &locale->category[gl_log2_lcmask_to_index (gl_log2_lc_mask (LC_CTYPE))];
  if (plc->is_c_locale)
    /* Implementation for the "C" locale.  */
    return C_FUNC (c);
#if HAVE_WINDOWS_LOCALE_T
# ifdef __MINGW32__
  /* mingw mistreats EOF by casting the argument to 'unsigned char'.
     <https://sourceforge.net/p/mingw-w64/mingw-w64/ci/b633824ecafdf52a76e6a205e6776b182978720d>  */
  if (c == EOF)
    return 0;
# endif
  return WINDOWS_FUNC (c, plc->system_locale);
#else
  /* Implementation for the global locale.  */
  {
    int ret;
# if HAVE_WORKING_USELOCALE
    locale_t saved_locale = uselocale (LC_GLOBAL_LOCALE);
# endif
    ret = GLOBAL_FUNC (c);
# if HAVE_WORKING_USELOCALE
    uselocale (saved_locale);
# endif
    return ret;
  }
#endif
}
