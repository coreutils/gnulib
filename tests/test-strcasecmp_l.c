/* Test of strcasecmp_l() function.
   Copyright (C) 2020-2025 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#include <config.h>

#include <strings.h>
#include <locale.h>

#include "signature.h"
SIGNATURE_CHECK (strcasecmp_l, int, (const char *, const char *, locale_t));

#include <stdio.h>

#include "macros.h"

static void
test_single_locale_common (locale_t locale)
{
  ASSERT (strcasecmp_l ("paragraph", "Paragraph", locale) == 0);

  ASSERT (strcasecmp_l ("paragrapH", "parAgRaph", locale) == 0);

  ASSERT (strcasecmp_l ("paragraph", "paraLyzed", locale) < 0);
  ASSERT (strcasecmp_l ("paraLyzed", "paragraph", locale) > 0);

  ASSERT (strcasecmp_l ("para", "paragraph", locale) < 0);
  ASSERT (strcasecmp_l ("paragraph", "para", locale) > 0);
}

int
main ()
{
  {
    locale_t locale = newlocale (LC_ALL_MASK, "C", NULL);
    ASSERT (locale != NULL);

    test_single_locale_common (locale);

    freelocale (locale);
  }
#if !MUSL_LIBC /* musl libc has no unibyte locales */
  {
# if defined _WIN32 && !defined __CYGWIN__
    locale_t locale = newlocale (LC_ALL_MASK, "French_France.1252", NULL);
# else
    locale_t locale = newlocale (LC_ALL_MASK, "fr_FR.ISO-8859-1", NULL);
    if (locale == NULL)
      locale = newlocale (LC_ALL_MASK, "fr_FR.ISO8859-1", NULL);
# endif
    if (locale != NULL)
      {
        test_single_locale_common (locale);

        /* Locale encoding is ISO-8859-1 or ISO-8859-15.  */

        /* U+00C9 LATIN CAPITAL LETTER E WITH ACUTE */
        /* U+00E9 LATIN SMALL LETTER E WITH ACUTE */
        ASSERT (strcasecmp_l ("Fej\311r", "Fej\351r", locale) == 0);
        ASSERT (strcasecmp_l ("Fej\351r", "Fej\311r", locale) == 0);
        ASSERT (strcasecmp_l ("Fejer", "Fej\311r", locale) < 0);
        ASSERT (strcasecmp_l ("Fej\311r", "Fejer", locale) > 0);

        /* Compare with U+00D7 MULTIPLICATION SIGN */
        ASSERT (strcasecmp_l ("Fej\311r", "Fej\327", locale) > 0);
        ASSERT (strcasecmp_l ("Fej\327", "Fej\311r", locale) < 0);
        ASSERT (strcasecmp_l ("Fej\351r", "Fej\327", locale) > 0);
        ASSERT (strcasecmp_l ("Fej\327", "Fej\351r", locale) < 0);

        freelocale (locale);
      }
  }
#endif

  return test_exit_status;
}
