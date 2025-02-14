/* Test of freeing a locale object.
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
#if !(defined __APPLE__ && defined __MACH__)
SIGNATURE_CHECK (freelocale, void, (locale_t));
#endif

#include "macros.h"

#if defined _WIN32 && !defined __CYGWIN__

# define ENGLISH  "English_United States"
# define FRENCH   "French_France"
# define ENCODING ".1252"

# define LOCALE1 ENGLISH ENCODING
# define LOCALE2 FRENCH ENCODING

#else

# define LOCALE1 "en_US.UTF-8"
# define LOCALE2 "fr_FR.UTF-8"

#endif

int
main ()
{
  {
    locale_t l1 = newlocale (LC_CTYPE_MASK, "C", NULL);
    ASSERT (l1 != NULL);
    freelocale (l1);
  }
  {
    locale_t l1 = newlocale (LC_MESSAGES_MASK, "C", NULL);
    ASSERT (l1 != NULL);
    freelocale (l1);
  }
  {
    locale_t l1 = newlocale (LC_ALL_MASK, "C", NULL);
    ASSERT (l1 != NULL);
    freelocale (l1);
  }
  {
    locale_t l1 = newlocale (LC_CTYPE_MASK, LOCALE1, NULL);
    if (l1 != NULL)
      freelocale (l1);
  }
  {
    locale_t l1 = newlocale (LC_MESSAGES_MASK, LOCALE1, NULL);
    if (l1 != NULL)
      freelocale (l1);
  }
  {
    locale_t l1 = newlocale (LC_ALL_MASK, LOCALE1, NULL);
    if (l1 != NULL)
      freelocale (l1);
  }
  {
    locale_t l1 = newlocale (LC_ALL_MASK, LOCALE1, NULL);
    if (l1 != NULL)
      {
        locale_t l2 = newlocale (LC_TIME_MASK, LOCALE2, l1);
        if (l2 != NULL)
          freelocale (l2);
        else
          freelocale (l1);
      }
  }
  {
    locale_t l1 = newlocale (LC_MESSAGES_MASK, LOCALE1, NULL);
    if (l1 != NULL)
      {
        locale_t l2 = newlocale (LC_TIME_MASK, LOCALE2, l1);
        if (l2 != NULL)
          freelocale (l2);
        else
          freelocale (l1);
      }
  }

  return test_exit_status;
}
