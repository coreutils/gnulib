/* Case-insensitive string comparison function for unibyte locales.
   Copyright (C) 1998-2025 Free Software Foundation, Inc.

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

/* Specification.  */
#include <strings.h>

#include <ctype.h>
#include <limits.h>
#include <string.h>

#include "c-strcase.h"

int
strcasecmp_l (const char *s1, const char *s2, locale_t locale)
{
#if GNULIB_defined_locale_t

  struct gl_locale_category_t *plc =
    &locale->category[gl_log2_lcmask_to_index (gl_log2_lc_mask (LC_CTYPE))];
  if (plc->is_c_locale)
    /* Implementation for the "C" locale.  */
    return c_strcasecmp (s1, s2);
# if HAVE_WINDOWS_LOCALE_T
  /* Documentation:
     <https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/stricmp-wcsicmp-mbsicmp-stricmp-l-wcsicmp-l-mbsicmp-l>  */
  return _stricmp_l (s1, s2, plc->system_locale);
# else
  /* Implementation for the global locale.  */
  {
    int ret;
#  if HAVE_WORKING_USELOCALE
    locale_t saved_locale = uselocale (LC_GLOBAL_LOCALE);
#  endif
    ret = strcasecmp (s1, s2);
#  if HAVE_WORKING_USELOCALE
    uselocale (saved_locale);
#  endif
    return ret;
  }
# endif

#else

  unsigned char c1, c2;

  if (s1 == s2)
    return 0;

  do
    {
      c1 = tolower_l ((unsigned char) *s1, locale);
      c2 = tolower_l ((unsigned char) *s2, locale);

      if (c1 == '\0')
        break;

      ++s1;
      ++s2;
    }
  while (c1 == c2);

  if (UCHAR_MAX <= INT_MAX)
    return c1 - c2;
  else
    /* On machines where 'char' and 'int' are types of the same size, the
       difference of two 'unsigned char' values - including the sign bit -
       doesn't fit in an 'int'.  */
    return _GL_CMP (c1, c2);

#endif
}
