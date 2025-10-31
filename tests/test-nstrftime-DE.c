/* Test of nstrftime in Germany.
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
# define LOCALE "German_Germany.65001"
#else
# define LOCALE "de_DE.UTF-8"
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
      fprintf (stderr, "Skipping test: Unicode locale for Germany is not installed\n");
      return 77;
    }

#if MUSL_LIBC
  fprintf (stderr, "Skipping test: system may not have localized month names\n");
  return 77;
#elif defined __OpenBSD__
  fprintf (stderr, "Skipping test: system does not have localized month names\n");
  return 77;
#else

  char buf[100];
  ptrdiff_t ret;
  /* Native Windows does not support dates before 1970-01-01.  */
# if !(defined _WIN32 && !defined __CYGWIN__)
  {
    DECLARE_TM (tm, 1969, 12, 28);

    ret = nstrftime (buf, sizeof (buf), "%Y-%m-%d",
                     &tm, (timezone_t) 0, 0);
    ASSERT (ret > 0);
    ASSERT (strcmp (buf, "1969-12-28") == 0);

    ret = nstrftime (buf, sizeof (buf), "%-d. %B %Y",
                     &tm, (timezone_t) 0, 0);
    ASSERT (ret > 0);
    ASSERT (strcmp (buf, "28. Dezember 1969") == 0);

    ret = nstrftime (buf, sizeof (buf), "%x",
                     &tm, (timezone_t) 0, 0);
    ASSERT (ret > 0);
    ASSERT (strcmp (buf, "28.12.1969") == 0
            || strcmp (buf, "28.12.69") == 0 /* musl, NetBSD, Solaris */);
  }
# endif
  {
    DECLARE_TM (tm, 2025, 3, 1);

    ret = nstrftime (buf, sizeof (buf), "%Y-%m-%d",
                     &tm, (timezone_t) 0, 0);
    ASSERT (ret > 0);
    ASSERT (strcmp (buf, "2025-03-01") == 0);

    ret = nstrftime (buf, sizeof (buf), "%-d. %B %Y",
                     &tm, (timezone_t) 0, 0);
    ASSERT (ret > 0);
    ASSERT (strcmp (buf, "1. März 2025") == 0);

    ret = nstrftime (buf, sizeof (buf), "%x",
                     &tm, (timezone_t) 0, 0);
    ASSERT (ret > 0);
    ASSERT (strcmp (buf, "01.03.2025") == 0
            || strcmp (buf, "01.03.25") == 0 /* musl, NetBSD, Solaris */);
  }

  return test_exit_status;
#endif
}
