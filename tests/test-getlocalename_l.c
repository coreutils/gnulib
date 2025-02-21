/* Test of getlocalename_l function.
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

/* Specification.  */
#include <locale.h>

#include "signature.h"
SIGNATURE_CHECK (getlocalename_l, const char *, (int, locale_t));

#include <string.h>

#include "macros.h"

#ifdef __HAIKU__
/* Work around Haiku bug <https://dev.haiku-os.org/ticket/18344>.  */
# define freelocale(loc) ((void) (loc))
#endif

/* The name that setlocale(,NULL) returns for the "C" locale.  */
#ifdef __HAIKU__
# define C_CANONICALIZED "POSIX"
#else
# define C_CANONICALIZED "C"
#endif

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
  /* Test the "C" locale.  */
  {
    const char *ret;
    locale_t locale = newlocale (LC_ALL_MASK, "C", NULL);
    ASSERT (locale != NULL);

    ret = getlocalename_l (LC_COLLATE, locale);
    ASSERT (strcmp (ret, "C") == 0);

    ret = getlocalename_l (LC_CTYPE, locale);
    ASSERT (strcmp (ret, "C") == 0);

    ret = getlocalename_l (LC_MESSAGES, locale);
    ASSERT (strcmp (ret, "C") == 0);

    ret = getlocalename_l (LC_MONETARY, locale);
    ASSERT (strcmp (ret, "C") == 0);

    ret = getlocalename_l (LC_NUMERIC, locale);
    ASSERT (strcmp (ret, "C") == 0);

    ret = getlocalename_l (LC_TIME, locale);
    ASSERT (strcmp (ret, "C") == 0);

    freelocale (locale);
  }

  /* Test an English locale.  */
  {
    locale_t locale1 = newlocale (LC_ALL_MASK, LOCALE1, NULL);
    if (locale1 != NULL)
      {
        const char *ret;

        ret = getlocalename_l (LC_COLLATE, locale1);
        ASSERT (strcmp (ret, LOCALE1) == 0);

        ret = getlocalename_l (LC_CTYPE, locale1);
        ASSERT (strcmp (ret, LOCALE1) == 0);

        ret = getlocalename_l (LC_MESSAGES, locale1);
        ASSERT (strcmp (ret, LOCALE1) == 0);

        ret = getlocalename_l (LC_MONETARY, locale1);
        ASSERT (strcmp (ret, LOCALE1) == 0);

        ret = getlocalename_l (LC_NUMERIC, locale1);
        ASSERT (strcmp (ret, LOCALE1) == 0);

        ret = getlocalename_l (LC_TIME, locale1);
        ASSERT (strcmp (ret, LOCALE1) == 0);

        freelocale (locale1);
      }
  }

  /* Test a mixed locale.  */
  {
    locale_t locale1 = newlocale (LC_ALL_MASK, LOCALE1, NULL);
    if (locale1 != NULL)
      {
        locale_t locale2 = newlocale (LC_COLLATE_MASK, LOCALE2, locale1);
        if (locale2 != NULL)
          {
            const char *ret;

            ret = getlocalename_l (LC_COLLATE, locale2);
            ASSERT (strcmp (ret, LOCALE2) == 0);

            ret = getlocalename_l (LC_CTYPE, locale2);
            ASSERT (strcmp (ret, LOCALE1) == 0);

            ret = getlocalename_l (LC_MESSAGES, locale2);
            ASSERT (strcmp (ret, LOCALE1) == 0);

            ret = getlocalename_l (LC_MONETARY, locale2);
            ASSERT (strcmp (ret, LOCALE1) == 0);

            ret = getlocalename_l (LC_NUMERIC, locale2);
            ASSERT (strcmp (ret, LOCALE1) == 0);

            ret = getlocalename_l (LC_TIME, locale2);
            ASSERT (strcmp (ret, LOCALE1) == 0);

            freelocale (locale2);
          }
        else
          freelocale (locale1);
      }
  }
  {
    locale_t locale1 = newlocale (LC_ALL_MASK, LOCALE1, NULL);
    if (locale1 != NULL)
      {
        locale_t locale2 = newlocale (LC_CTYPE_MASK, LOCALE2, locale1);
        if (locale2 != NULL)
          {
            const char *ret;

            ret = getlocalename_l (LC_COLLATE, locale2);
            ASSERT (strcmp (ret, LOCALE1) == 0);

            ret = getlocalename_l (LC_CTYPE, locale2);
            ASSERT (strcmp (ret, LOCALE2) == 0);

            ret = getlocalename_l (LC_MESSAGES, locale2);
            ASSERT (strcmp (ret, LOCALE1) == 0);

            ret = getlocalename_l (LC_MONETARY, locale2);
            ASSERT (strcmp (ret, LOCALE1) == 0);

            ret = getlocalename_l (LC_NUMERIC, locale2);
            ASSERT (strcmp (ret, LOCALE1) == 0);

            ret = getlocalename_l (LC_TIME, locale2);
            ASSERT (strcmp (ret, LOCALE1) == 0);

            freelocale (locale2);
          }
        else
          freelocale (locale1);
      }
  }
  {
    locale_t locale1 = newlocale (LC_ALL_MASK, LOCALE1, NULL);
    if (locale1 != NULL)
      {
        locale_t locale2 = newlocale (LC_MESSAGES_MASK, LOCALE2, locale1);
        if (locale2 != NULL)
          {
            const char *ret;

            ret = getlocalename_l (LC_COLLATE, locale2);
            ASSERT (strcmp (ret, LOCALE1) == 0);

            ret = getlocalename_l (LC_CTYPE, locale2);
            ASSERT (strcmp (ret, LOCALE1) == 0);

            ret = getlocalename_l (LC_MESSAGES, locale2);
            ASSERT (strcmp (ret, LOCALE2) == 0);

            ret = getlocalename_l (LC_MONETARY, locale2);
            ASSERT (strcmp (ret, LOCALE1) == 0);

            ret = getlocalename_l (LC_NUMERIC, locale2);
            ASSERT (strcmp (ret, LOCALE1) == 0);

            ret = getlocalename_l (LC_TIME, locale2);
            ASSERT (strcmp (ret, LOCALE1) == 0);

            freelocale (locale2);
          }
        else
          freelocale (locale1);
      }
  }
  {
    locale_t locale1 = newlocale (LC_ALL_MASK, LOCALE1, NULL);
    if (locale1 != NULL)
      {
        locale_t locale2 = newlocale (LC_MONETARY_MASK, LOCALE2, locale1);
        if (locale2 != NULL)
          {
            const char *ret;

            ret = getlocalename_l (LC_COLLATE, locale2);
            ASSERT (strcmp (ret, LOCALE1) == 0);

            ret = getlocalename_l (LC_CTYPE, locale2);
            ASSERT (strcmp (ret, LOCALE1) == 0);

            ret = getlocalename_l (LC_MESSAGES, locale2);
            ASSERT (strcmp (ret, LOCALE1) == 0);

            ret = getlocalename_l (LC_MONETARY, locale2);
            ASSERT (strcmp (ret, LOCALE2) == 0);

            ret = getlocalename_l (LC_NUMERIC, locale2);
            ASSERT (strcmp (ret, LOCALE1) == 0);

            ret = getlocalename_l (LC_TIME, locale2);
            ASSERT (strcmp (ret, LOCALE1) == 0);

            freelocale (locale2);
          }
        else
          freelocale (locale1);
      }
  }
  {
    locale_t locale1 = newlocale (LC_ALL_MASK, LOCALE1, NULL);
    if (locale1 != NULL)
      {
        locale_t locale2 = newlocale (LC_NUMERIC_MASK, LOCALE2, locale1);
        if (locale2 != NULL)
          {
            const char *ret;

            ret = getlocalename_l (LC_COLLATE, locale2);
            ASSERT (strcmp (ret, LOCALE1) == 0);

            ret = getlocalename_l (LC_CTYPE, locale2);
            ASSERT (strcmp (ret, LOCALE1) == 0);

            ret = getlocalename_l (LC_MESSAGES, locale2);
            ASSERT (strcmp (ret, LOCALE1) == 0);

            ret = getlocalename_l (LC_MONETARY, locale2);
            ASSERT (strcmp (ret, LOCALE1) == 0);

            ret = getlocalename_l (LC_NUMERIC, locale2);
            ASSERT (strcmp (ret, LOCALE2) == 0);

            ret = getlocalename_l (LC_TIME, locale2);
            ASSERT (strcmp (ret, LOCALE1) == 0);

            freelocale (locale2);
          }
        else
          freelocale (locale1);
      }
  }
  {
    locale_t locale1 = newlocale (LC_ALL_MASK, LOCALE1, NULL);
    if (locale1 != NULL)
      {
        locale_t locale2 = newlocale (LC_TIME_MASK, LOCALE2, locale1);
        if (locale2 != NULL)
          {
            const char *ret;

            ret = getlocalename_l (LC_COLLATE, locale2);
            ASSERT (strcmp (ret, LOCALE1) == 0);

            ret = getlocalename_l (LC_CTYPE, locale2);
            ASSERT (strcmp (ret, LOCALE1) == 0);

            ret = getlocalename_l (LC_MESSAGES, locale2);
            ASSERT (strcmp (ret, LOCALE1) == 0);

            ret = getlocalename_l (LC_MONETARY, locale2);
            ASSERT (strcmp (ret, LOCALE1) == 0);

            ret = getlocalename_l (LC_NUMERIC, locale2);
            ASSERT (strcmp (ret, LOCALE1) == 0);

            ret = getlocalename_l (LC_TIME, locale2);
            ASSERT (strcmp (ret, LOCALE2) == 0);

            freelocale (locale2);
          }
        else
          freelocale (locale1);
      }
  }

  /* Test the global locale.  */
  {
    const char *ret;

    ret = getlocalename_l (LC_COLLATE, LC_GLOBAL_LOCALE);
    ASSERT (strcmp (ret, C_CANONICALIZED) == 0);

    ret = getlocalename_l (LC_CTYPE, LC_GLOBAL_LOCALE);
    ASSERT (strcmp (ret, C_CANONICALIZED) == 0);

    ret = getlocalename_l (LC_MESSAGES, LC_GLOBAL_LOCALE);
    ASSERT (strcmp (ret, C_CANONICALIZED) == 0);

    ret = getlocalename_l (LC_MONETARY, LC_GLOBAL_LOCALE);
    ASSERT (strcmp (ret, C_CANONICALIZED) == 0);

    ret = getlocalename_l (LC_NUMERIC, LC_GLOBAL_LOCALE);
    ASSERT (strcmp (ret, C_CANONICALIZED) == 0);

    ret = getlocalename_l (LC_TIME, LC_GLOBAL_LOCALE);
    ASSERT (strcmp (ret, C_CANONICALIZED) == 0);
  }

  /* Skip this part on OpenBSD <= 6.1.  */
#if !(defined __OpenBSD__ && !HAVE_NEWLOCALE)
  if (setlocale (LC_ALL, LOCALE1) != NULL)
    {
      const char *ret;

      ret = getlocalename_l (LC_COLLATE, LC_GLOBAL_LOCALE);
      ASSERT (strcmp (ret, LOCALE1) == 0);

      ret = getlocalename_l (LC_CTYPE, LC_GLOBAL_LOCALE);
      ASSERT (strcmp (ret, LOCALE1) == 0);

      ret = getlocalename_l (LC_MESSAGES, LC_GLOBAL_LOCALE);
      ASSERT (strcmp (ret, LOCALE1) == 0);

      ret = getlocalename_l (LC_MONETARY, LC_GLOBAL_LOCALE);
      ASSERT (strcmp (ret, LOCALE1) == 0);

      ret = getlocalename_l (LC_NUMERIC, LC_GLOBAL_LOCALE);
      ASSERT (strcmp (ret, LOCALE1) == 0);

      ret = getlocalename_l (LC_TIME, LC_GLOBAL_LOCALE);
      ASSERT (strcmp (ret, LOCALE1) == 0);
    }
#endif

  return 0;
}
