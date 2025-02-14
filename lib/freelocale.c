/* Free a locale object.
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

#include <config.h>

/* Specification.  */
#include <locale.h>

#include <stdlib.h>

void
freelocale (locale_t locale)
{
  int i;
  for (i = 6; --i >= 0; )
    {
#if HAVE_WINDOWS_LOCALE_T
      if (!(i == gl_log2_lcmask_to_index (gl_log2_lc_mask (LC_MESSAGES))
            || locale->category[i].is_c_locale))
        /* Documentation:
           <https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/free-locale>  */
        _free_locale (locale->category[i].system_locale);
#endif
      free (locale->category[i].name);
    }
  free (locale);
}
