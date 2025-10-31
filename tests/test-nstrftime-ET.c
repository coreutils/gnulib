/* Test of nstrftime in Ethiopia.
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
# define LOCALE1 "Amharic_Ethiopia.65001"
# define LOCALE2 NULL
#else
# define LOCALE1 "am_ET.UTF-8"
# define LOCALE2 "am_ET"
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
      fprintf (stderr, "Skipping test: Unicode locale for Ethiopia is not installed\n");
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
    DECLARE_TM (tm, 1930, 11, 2);

    ret = nstrftime (buf, sizeof (buf), "%Y-%m-%d",
                     &tm, (timezone_t) 0, 0);
    ASSERT (ret > 0);
    ASSERT (strcmp (buf, "1923-02-23") == 0);

    ret = nstrftime (buf, sizeof (buf), "%-d %B %Y",
                     &tm, (timezone_t) 0, 0);
    ASSERT (ret > 0);
    ASSERT (strcmp (buf, "23 ጥቅምት 1923") == 0);

    ret = nstrftime (buf, sizeof (buf), "%x",
                     &tm, (timezone_t) 0, 0);
    ASSERT (ret > 0);
    ASSERT (strcmp (buf, "23/02/1923") == 0);
  }
  {
    DECLARE_TM (tm, 1969, 12, 28);

    ret = nstrftime (buf, sizeof (buf), "%Y-%m-%d",
                     &tm, (timezone_t) 0, 0);
    ASSERT (ret > 0);
    ASSERT (strcmp (buf, "1962-04-19") == 0);

    ret = nstrftime (buf, sizeof (buf), "%-d %B %Y",
                     &tm, (timezone_t) 0, 0);
    ASSERT (ret > 0);
    ASSERT (strcmp (buf, "19 ታኅሣሥ 1962") == 0);

    ret = nstrftime (buf, sizeof (buf), "%x",
                     &tm, (timezone_t) 0, 0);
    ASSERT (ret > 0);
    ASSERT (strcmp (buf, "19/04/1962") == 0);
  }
# endif
  {
    DECLARE_TM (tm, 2025, 3, 1);

    ret = nstrftime (buf, sizeof (buf), "%Y-%m-%d",
                     &tm, (timezone_t) 0, 0);
    ASSERT (ret > 0);
    ASSERT (strcmp (buf, "2017-06-22") == 0);

    ret = nstrftime (buf, sizeof (buf), "%-d %B %Y",
                     &tm, (timezone_t) 0, 0);
    ASSERT (ret > 0);
    ASSERT (strcmp (buf, "22 የካቲት 2017") == 0);

    ret = nstrftime (buf, sizeof (buf), "%x",
                     &tm, (timezone_t) 0, 0);
    ASSERT (ret > 0);
    ASSERT (strcmp (buf, "22/06/2017") == 0);
  }

  return test_exit_status;
#endif
}
