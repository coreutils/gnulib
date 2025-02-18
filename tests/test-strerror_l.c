/* Test of strerror_l() function.
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

#include <string.h>
#include <locale.h>

#include "signature.h"
SIGNATURE_CHECK (strerror_l, char *, (int, locale_t));

#include <errno.h>
#include <stdio.h>

#include "xalloc.h"
#include "macros.h"

#if defined _WIN32 && !defined __CYGWIN__

# define FRENCH   "French_France"
# define GERMAN   "German_Germany"
# define ENCODING ".1252"

# define LOCALE1 FRENCH ENCODING
# define LOCALE2 GERMAN ENCODING

#else

# define LOCALE1 "fr_FR.UTF-8"
# define LOCALE2 "de_DE.UTF-8"

#endif

int
main ()
{
  char *c_message;
  char *fr_message;

  c_message = xstrdup (strerror (ERANGE));

  if (setlocale (LC_ALL, LOCALE1) == NULL)
    {
      fprintf (stderr, "Skipping test: French locale %s not installed.\n", LOCALE1);
      return 77;
    }
  fr_message = xstrdup (strerror (ERANGE));
  if (strcmp (fr_message, c_message) == 0)
    {
      fprintf (stderr, "Skipping test: error descriptions are not localized.\n");
      return 77;
    }

  {
    locale_t l1 = newlocale (LC_ALL_MASK, "C", NULL);
    ASSERT (l1 != NULL);
    char *message = strerror_l (ERANGE, l1);
    ASSERT (message != NULL);
    ASSERT (strcmp (message, c_message) == 0);
    freelocale (l1);
  }
  {
    locale_t l1 = newlocale (LC_ALL_MASK, LOCALE2, NULL);
    if (l1 == NULL)
      {
        fprintf (stderr, "Skipping test: German locale %s not installed or not supported by newlocale().\n", LOCALE2);
        return 77;
      }
    char *message = strerror_l (ERANGE, l1);
    ASSERT (message != NULL);
    /* German is neither English nor French.  */
    ASSERT (strcmp (message, c_message) != 0);
    ASSERT (strcmp (message, fr_message) != 0);
    freelocale (l1);
  }

  return test_exit_status;
}
