/* Test of nstrftime in Iran.
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
#include "strftime.h"

#include <locale.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "localcharset.h"
#include "macros.h"

#if defined _WIN32 && !defined __CYGWIN__
# define LOCALE1 "Persian_Iran.65001"
# define LOCALE2 NULL
#else
# define LOCALE1 "fa_IR.UTF-8"
# define LOCALE2 "fa_IR"
#endif

#define DECLARE_TM(variable, greg_year, greg_month, greg_day) \
  struct tm variable =                          \
    {                                           \
      .tm_year = (greg_year) - 1900,            \
      .tm_mon = (greg_month) - 1,               \
      .tm_mday = (greg_day),                    \
      .tm_hour = 12, .tm_min = 34, .tm_sec = 56 \
    };                                          \
  /* Fill the other fields.  */                 \
  time_t tt = timegm (&variable);               \
  gmtime_r (&tt, &variable)/*;*/

int
main ()
{
  if (((setenv ("LC_ALL", LOCALE1, 1),
        (setlocale (LC_ALL, "") == NULL
         || strcmp (setlocale (LC_ALL, NULL), "C") == 0))
       && (LOCALE2 == NULL
           || (setenv ("LC_ALL", LOCALE2, 1),
               (setlocale (LC_ALL, "") == NULL
                || strcmp (setlocale (LC_ALL, NULL), "C") == 0))))
      || strcmp (locale_charset (), "UTF-8") != 0)
    {
      fprintf (stderr, "Skipping test: Unicode locale for Iran is not installed\n");
      return 77;
    }

#if defined __OpenBSD__ || defined _AIX || defined __ANDROID__
  fprintf (stderr, "Skipping test: determining the locale name is not worth it on this platform\n");
  return 77;
#else

  char buf[100];
  ptrdiff_t ret;
  /* Native Windows does not support dates before 1970-01-01.  */
# if !(defined _WIN32 && !defined __CYGWIN__)
  {
    DECLARE_TM (tm, 1967, 10, 26);

    ret = nstrftime (buf, sizeof (buf), "%Y-%m-%d",
                     &tm, (timezone_t) 0, 0);
    ASSERT (ret > 0);
    ASSERT (strcmp (buf, "1346-08-04") == 0);

    ret = nstrftime (buf, sizeof (buf), "%-d %B %Y",
                     &tm, (timezone_t) 0, 0);
    ASSERT (ret > 0);
    ASSERT (strcmp (buf, "4 آبان 1346") == 0);

    ret = nstrftime (buf, sizeof (buf), "%x",
                     &tm, (timezone_t) 0, 0);
    ASSERT (ret > 0);
    ASSERT (strcmp (buf, "۱۳۴۶/۸/۴") == 0);
  }
  {
    DECLARE_TM (tm, 1969, 12, 28);

    ret = nstrftime (buf, sizeof (buf), "%Y-%m-%d",
                     &tm, (timezone_t) 0, 0);
    ASSERT (ret > 0);
    ASSERT (strcmp (buf, "1348-10-07") == 0);

    ret = nstrftime (buf, sizeof (buf), "%-d %B %Y",
                     &tm, (timezone_t) 0, 0);
    ASSERT (ret > 0);
    ASSERT (strcmp (buf, "7 دی 1348") == 0);

    ret = nstrftime (buf, sizeof (buf), "%x",
                     &tm, (timezone_t) 0, 0);
    ASSERT (ret > 0);
    ASSERT (strcmp (buf, "۱۳۴۸/۱۰/۷") == 0);
  }
# endif
  /* Verify that 1403 is a leap year and 1404 is not.  */
  {
    DECLARE_TM (tm, 2024, 3, 19);

    ret = nstrftime (buf, sizeof (buf), "%Y-%m-%d",
                     &tm, (timezone_t) 0, 0);
    ASSERT (ret > 0);
    ASSERT (strcmp (buf, "1402-12-29") == 0);

    ret = nstrftime (buf, sizeof (buf), "%-d %B %Y",
                     &tm, (timezone_t) 0, 0);
    ASSERT (ret > 0);
    ASSERT (strcmp (buf, "29 اسفند 1402") == 0);

    ret = nstrftime (buf, sizeof (buf), "%x",
                     &tm, (timezone_t) 0, 0);
    ASSERT (ret > 0);
    ASSERT (strcmp (buf, "۱۴۰۲/۱۲/۲۹") == 0);
  }
  {
    DECLARE_TM (tm, 2024, 3, 22);

    ret = nstrftime (buf, sizeof (buf), "%Y-%m-%d",
                     &tm, (timezone_t) 0, 0);
    ASSERT (ret > 0);
    ASSERT (strcmp (buf, "1403-01-03") == 0);

    ret = nstrftime (buf, sizeof (buf), "%-d %B %Y",
                     &tm, (timezone_t) 0, 0);
    ASSERT (ret > 0);
    ASSERT (strcmp (buf, "3 فروردین 1403") == 0);

    ret = nstrftime (buf, sizeof (buf), "%x",
                     &tm, (timezone_t) 0, 0);
    ASSERT (ret > 0);
    ASSERT (strcmp (buf, "۱۴۰۳/۱/۳") == 0);
  }
  {
    DECLARE_TM (tm, 2025, 3, 19);

    ret = nstrftime (buf, sizeof (buf), "%Y-%m-%d",
                     &tm, (timezone_t) 0, 0);
    ASSERT (ret > 0);
    ASSERT (strcmp (buf, "1403-12-29") == 0);

    ret = nstrftime (buf, sizeof (buf), "%-d %B %Y",
                     &tm, (timezone_t) 0, 0);
    ASSERT (ret > 0);
    ASSERT (strcmp (buf, "29 اسفند 1403") == 0);

    ret = nstrftime (buf, sizeof (buf), "%x",
                     &tm, (timezone_t) 0, 0);
    ASSERT (ret > 0);
    ASSERT (strcmp (buf, "۱۴۰۳/۱۲/۲۹") == 0);
  }
  {
    DECLARE_TM (tm, 2025, 3, 22);

    ret = nstrftime (buf, sizeof (buf), "%Y-%m-%d",
                     &tm, (timezone_t) 0, 0);
    ASSERT (ret > 0);
    ASSERT (strcmp (buf, "1404-01-02") == 0);

    ret = nstrftime (buf, sizeof (buf), "%-d %B %Y",
                     &tm, (timezone_t) 0, 0);
    ASSERT (ret > 0);
    ASSERT (strcmp (buf, "2 فروردین 1404") == 0);

    ret = nstrftime (buf, sizeof (buf), "%x",
                     &tm, (timezone_t) 0, 0);
    ASSERT (ret > 0);
    ASSERT (strcmp (buf, "۱۴۰۴/۱/۲") == 0);
  }

  return test_exit_status;
#endif
}
