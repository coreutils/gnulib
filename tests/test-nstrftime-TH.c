/* Test of nstrftime in Thailand.
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
# define LOCALE "Thai_Thailand.65001"
#else
# define LOCALE "th_TH.UTF-8"
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
  setenv ("LC_ALL", LOCALE, 1);
  if (setlocale (LC_ALL, "") == NULL
      || strcmp (setlocale (LC_ALL, NULL), "C") == 0
      || strcmp (locale_charset (), "UTF-8") != 0)
    {
      fprintf (stderr, "Skipping test: Unicode locale for Thailand is not installed\n");
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
    DECLARE_TM (tm, 1939, 6, 23);

    ret = nstrftime (buf, sizeof (buf), "%Y-%m-%d",
                     &tm, (timezone_t) 0, 0);
    ASSERT (ret > 0);
    ASSERT (strcmp (buf, "2482-03-23") == 0);

    ret = nstrftime (buf, sizeof (buf), "%-d. %B %Y",
                     &tm, (timezone_t) 0, 0);
    ASSERT (ret > 0);
    ASSERT (strcmp (buf, "23. มิถุนายน 2482") == 0);

    ret = nstrftime (buf, sizeof (buf), "%x",
                     &tm, (timezone_t) 0, 0);
    ASSERT (ret > 0);
    ASSERT (strcmp (buf, "23/03/2482") == 0);
  }
  {
    DECLARE_TM (tm, 1969, 12, 28);

    ret = nstrftime (buf, sizeof (buf), "%Y-%m-%d",
                     &tm, (timezone_t) 0, 0);
    ASSERT (ret > 0);
    ASSERT (strcmp (buf, "2512-12-28") == 0);

    ret = nstrftime (buf, sizeof (buf), "%-d. %B %Y",
                     &tm, (timezone_t) 0, 0);
    ASSERT (ret > 0);
    ASSERT (strcmp (buf, "28. ธันวาคม 2512") == 0);

    ret = nstrftime (buf, sizeof (buf), "%x",
                     &tm, (timezone_t) 0, 0);
    ASSERT (ret > 0);
    ASSERT (strcmp (buf, "28/12/2512") == 0);
  }
# endif
  {
    DECLARE_TM (tm, 2025, 3, 1);

    ret = nstrftime (buf, sizeof (buf), "%Y-%m-%d",
                     &tm, (timezone_t) 0, 0);
    ASSERT (ret > 0);
    ASSERT (strcmp (buf, "2568-03-01") == 0);

    ret = nstrftime (buf, sizeof (buf), "%-d. %B %Y",
                     &tm, (timezone_t) 0, 0);
    ASSERT (ret > 0);
    ASSERT (strcmp (buf, "1. มีนาคม 2568") == 0);

    ret = nstrftime (buf, sizeof (buf), "%x",
                     &tm, (timezone_t) 0, 0);
    ASSERT (ret > 0);
    ASSERT (strcmp (buf, "01/03/2568") == 0);
  }

  return test_exit_status;
#endif
}
