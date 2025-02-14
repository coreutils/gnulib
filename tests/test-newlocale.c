/* Test of creating a locale object.
   Copyright (C) 2025 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2025.  */

#include <config.h>

#include <locale.h>

#include "signature.h"
SIGNATURE_CHECK (newlocale, locale_t, (int, const char *, locale_t));

#if HAVE_NL_LANGINFO_L
# include <langinfo.h>
#endif

#include "macros.h"

#if defined _WIN32 && !defined __CYGWIN__

# define ENGLISH  "English_United States"
# define FRENCH   "French_France"
# define GERMAN   "German_Germany"
# define ENCODING ".1252"

# define LOCALE1 ENGLISH ENCODING
# define LOCALE2 FRENCH ENCODING
# define LOCALE3 GERMAN ENCODING

#else

# define LOCALE1 "en_US.UTF-8"
# define LOCALE2 "fr_FR.UTF-8"
# define LOCALE3 "de_DE.UTF-8"

#endif

int
main ()
{
  {
    locale_t l1 = newlocale (LC_TIME_MASK, LOCALE1, NULL);
    locale_t l2 = newlocale (LC_MESSAGES_MASK, LOCALE2, l1);
    locale_t l3 = newlocale (LC_TIME_MASK, LOCALE3, l2);
    (void) l3;
  }

#if HAVE_NL_LANGINFO_L
  /* Verify that when the base argument is NULL, "the data for all sections
     not requested by category_mask shall be taken from the POSIX locale".  */
  {
    locale_t l1 = newlocale (LC_TIME_MASK, LOCALE1, NULL);
    if (l1 != NULL)
      {
        const char *radixchar1 = nl_langinfo_l (RADIXCHAR, l1);
        ASSERT (*radixchar1 == '.');
        if (setlocale (LC_ALL, LOCALE2) != NULL)
          {
            radixchar1 = nl_langinfo_l (RADIXCHAR, l1);
            ASSERT (*radixchar1 == '.');
            locale_t l1a = newlocale (LC_TIME_MASK, LOCALE1, NULL);
            const char *radixchar1a = nl_langinfo_l (RADIXCHAR, l1a);
            ASSERT (*radixchar1a == '.');
          }
      }
  }
#endif

  return test_exit_status;
}
