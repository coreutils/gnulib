/* Language code of current locale.
   Copyright (C) 2009-2010 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2009.

   This program is free software: you can redistribute it and/or modify it
   under the terms of the GNU Lesser General Public License as published
   by the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include <config.h>

/* Specification.  */
#include "unicase.h"

#include <locale.h>
#include <string.h>

#include "localename.h"

#include "unicase/locale-languages.h"

const char *
uc_locale_language (void)
{
  const char *locale_name = gl_locale_name (LC_CTYPE, "LC_CTYPE");
  const char *p;

  /* Find the end of the language portion.  */
  p = locale_name;
  while (*p != '\0' && *p != '_' && *p != '.' && *p != '@')
    p++;

  /* Return a statically allocated pointer to the language portion, so that the
     caller of this function does not need to free() the result.
     We could use a dynamically built unification table.  But given that the
     set of possible languages is small, it is easier (and excludes ENOMEM
     error conditions) to allocate the unification table statically.  */
  if (p != locale_name)
    {
      const char *language =
        uc_locale_languages_lookup (locale_name, p - locale_name);
      if (language != NULL)
        return language;
    }

  return "";
}
