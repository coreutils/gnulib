/* Test of getdate() function.
   Copyright (C) 2023-2026 Free Software Foundation, Inc.

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

/* Written by Bruno Haible.  */

#include <config.h>

/* Specification.  */
#include <time.h>

#include "signature.h"
SIGNATURE_CHECK (getdate, struct tm *, (const char *));

#include <stdio.h>

#include "macros.h"

int
main (void)
{
  /* Preparations.  */
  {
    const char *filename = "datemsk";

    FILE *fp = fopen (filename, "w");
    ASSERT (fp != NULL);
    const char *contents =
      "%m\n"
      "%A %B %d, %Y, %H:%M:%S\n"
      "%A\n"
      "%B\n"
      "%m/%d/%y %I %p\n"
      "%d,%m,%Y %H:%M\n"
      "%m/%d/%y\n"
      "%d.%m.%y\n"
      "%y-%m-%d\n"
      "%A %H:%M:%S\n"
      "%a\n"
      "%b %a\n"
      "%b %a %Y\n"
      "%a %H\n"
      "%b %H:%S\n"
      "%H:%M\n"
      ;
    ASSERT (fputs (contents, fp) != EOF);
    ASSERT (fclose (fp) == 0);

    ASSERT (setenv ("DATEMSK", filename, 1) == 0);
  }

  /* This test case originally failed on macOS, NetBSD, Solaris 11 OmniOS.  */
  {
    struct tm *t = getdate ("10/1/87 4 PM");
    ASSERT (t != NULL);
    ASSERT (t->tm_year == 1987 - 1900
            && t->tm_mon == 10 - 1
            && t->tm_mday == 1
            && t->tm_hour == 16 && t->tm_min == 0 && t->tm_sec == 0);
  }

  /* This test case originally failed on Solaris 11 OmniOS.  */
  {
    struct tm *t = getdate ("Friday September 18, 1987, 10:30:30");
    ASSERT (t != NULL);
    ASSERT (t->tm_year == 1987 - 1900
            && t->tm_mon == 9 - 1
            && t->tm_mday == 18
            && t->tm_hour == 10 && t->tm_min == 30 && t->tm_sec == 30);
  }

  /* This test case originally failed on musl libc, NetBSD, Solaris 11 OmniOS.  */
  {
    struct tm *t = getdate ("24,9,1986 10:30");
    ASSERT (t != NULL);
    ASSERT (t->tm_year == 1986 - 1900
            && t->tm_mon == 9 - 1
            && t->tm_mday == 24
            && t->tm_hour == 10 && t->tm_min == 30 && t->tm_sec == 0);
  }

  /* This test case originally failed on NetBSD, Solaris 11 OmniOS.  */
  {
    struct tm *t = getdate ("11/27/86");
    ASSERT (t != NULL);
    ASSERT (t->tm_year == 1986 - 1900
            && t->tm_mon == 11 - 1
            && t->tm_mday == 27
            && t->tm_hour >= 0 && t->tm_hour < 24
            && t->tm_min >= 0 && t->tm_min < 60
            && t->tm_sec >= 0 && t->tm_sec < 60);
  }

  /* This test case originally failed on NetBSD.  */
  {
    struct tm *t = getdate ("27.11.86");
    ASSERT (t != NULL);
    ASSERT (t->tm_year == 1986 - 1900
            && t->tm_mon == 11 - 1
            && t->tm_mday == 27
            && t->tm_hour >= 0 && t->tm_hour < 24
            && t->tm_min >= 0 && t->tm_min < 60
            && t->tm_sec >= 0 && t->tm_sec < 60);
  }

  /* This test case originally failed on NetBSD.  */
  {
    struct tm *t = getdate ("86-11-27");
    ASSERT (t != NULL);
    ASSERT (t->tm_year == 1986 - 1900
            && t->tm_mon == 11 - 1
            && t->tm_mday == 27
            && t->tm_hour >= 0 && t->tm_hour < 24
            && t->tm_min >= 0 && t->tm_min < 60
            && t->tm_sec >= 0 && t->tm_sec < 60);
  }

  /* This test case originally failed on NetBSD, Solaris 11 OmniOS.  */
  {
    struct tm *t = getdate ("Friday 12:00:00");
    ASSERT (t != NULL);
    ASSERT (t->tm_year >= 1970 - 1900
            && t->tm_mon >= 0 && t->tm_mon < 12
            && t->tm_mday >= 1 && t->tm_mday <= 31
            && t->tm_wday == 5
            && t->tm_hour == 12 && t->tm_min == 0 && t->tm_sec == 0);
  }

  /* This test case originally failed on Solaris 11 OmniOS.  */
  {
    struct tm *t = getdate ("Friday");
    ASSERT (t != NULL);
    ASSERT (t->tm_year >= 1970 - 1900
            && t->tm_mon >= 0 && t->tm_mon < 12
            && t->tm_mday >= 1 && t->tm_mday <= 31
            && t->tm_wday == 5
            && t->tm_hour >= 0 && t->tm_hour < 24
            && t->tm_min >= 0 && t->tm_min < 60
            && t->tm_sec >= 0 && t->tm_sec < 60);
  }

  /* This test case originally failed on Solaris 11 OmniOS.  */
  {
    struct tm *t = getdate ("Mon");
    ASSERT (t != NULL);
    ASSERT (t->tm_year >= 1970 - 1900
            && t->tm_mon >= 0 && t->tm_mon < 12
            && t->tm_mday >= 1 && t->tm_mday <= 31
            && t->tm_wday == 1
            && t->tm_hour >= 0 && t->tm_hour < 24
            && t->tm_min >= 0 && t->tm_min < 60
            && t->tm_sec >= 0 && t->tm_sec < 60);
  }

  /* This test case originally failed on Solaris 11 OmniOS.  */
  {
    struct tm *t = getdate ("Sun");
    ASSERT (t != NULL);
    ASSERT (t->tm_year >= 1970 - 1900
            && t->tm_mon >= 0 && t->tm_mon < 12
            && t->tm_mday >= 1 && t->tm_mday <= 31
            && t->tm_wday == 0
            && t->tm_hour >= 0 && t->tm_hour < 24
            && t->tm_min >= 0 && t->tm_min < 60
            && t->tm_sec >= 0 && t->tm_sec < 60);
  }

  /* This test case originally failed on Solaris 11 OmniOS.  */
  {
    struct tm *t = getdate ("Fri");
    ASSERT (t != NULL);
    ASSERT (t->tm_year >= 1970 - 1900
            && t->tm_mon >= 0 && t->tm_mon < 12
            && t->tm_mday >= 1 && t->tm_mday <= 31
            && t->tm_wday == 5
            && t->tm_hour >= 0 && t->tm_hour < 24
            && t->tm_min >= 0 && t->tm_min < 60
            && t->tm_sec >= 0 && t->tm_sec < 60);
  }

  /* This test case originally failed on Solaris 11 OmniOS.  */
  {
    struct tm *t = getdate ("September");
    ASSERT (t != NULL);
    ASSERT (t->tm_year >= 1970 - 1900
            && t->tm_mon == 9 - 1
            && t->tm_mday >= 1 && t->tm_mday <= 30
            && t->tm_hour >= 0 && t->tm_hour < 24
            && t->tm_min >= 0 && t->tm_min < 60
            && t->tm_sec >= 0 && t->tm_sec < 60);
  }

  /* This test case originally failed on Solaris 11 OmniOS.  */
  {
    struct tm *t = getdate ("January");
    ASSERT (t != NULL);
    ASSERT (t->tm_year >= 1970 - 1900
            && t->tm_mon == 1 - 1
            && t->tm_mday >= 1 && t->tm_mday <= 31
            && t->tm_hour >= 0 && t->tm_hour < 24
            && t->tm_min >= 0 && t->tm_min < 60
            && t->tm_sec >= 0 && t->tm_sec < 60);
  }

  /* This test case originally failed on Solaris 11 OmniOS.  */
  {
    struct tm *t = getdate ("December");
    ASSERT (t != NULL);
    ASSERT (t->tm_year >= 1970 - 1900
            && t->tm_mon == 12 - 1
            && t->tm_mday >= 1 && t->tm_mday <= 31
            && t->tm_hour >= 0 && t->tm_hour < 24
            && t->tm_min >= 0 && t->tm_min < 60
            && t->tm_sec >= 0 && t->tm_sec < 60);
  }

  /* This test case originally failed on NetBSD, Solaris 11 OmniOS.  */
  {
    struct tm *t = getdate ("Sep Mon");
    ASSERT (t != NULL);
    ASSERT (t->tm_year >= 1970 - 1900
            && t->tm_mon == 9 - 1
            && t->tm_mday >= 1 && t->tm_mday <= 30
            && t->tm_wday == 1
            && t->tm_hour >= 0 && t->tm_hour < 24
            && t->tm_min >= 0 && t->tm_min < 60
            && t->tm_sec >= 0 && t->tm_sec < 60);
  }

  /* This test case originally failed on NetBSD, Solaris 11 OmniOS.  */
  {
    struct tm *t = getdate ("Jan Fri");
    ASSERT (t != NULL);
    ASSERT (t->tm_year >= 1970 - 1900
            && t->tm_mon == 1 - 1
            && t->tm_mday >= 1 && t->tm_mday <= 31
            && t->tm_wday == 5
            && t->tm_hour >= 0 && t->tm_hour < 24
            && t->tm_min >= 0 && t->tm_min < 60
            && t->tm_sec >= 0 && t->tm_sec < 60);
  }

  /* This test case originally failed on NetBSD, Solaris 11 OmniOS.  */
  {
    struct tm *t = getdate ("Dec Mon");
    ASSERT (t != NULL);
    ASSERT (t->tm_year >= 1970 - 1900
            && t->tm_mon == 12 - 1
            && t->tm_mday >= 1 && t->tm_mday <= 31
            && t->tm_wday == 1
            && t->tm_hour >= 0 && t->tm_hour < 24
            && t->tm_min >= 0 && t->tm_min < 60
            && t->tm_sec >= 0 && t->tm_sec < 60);
  }

  /* This test case originally failed on NetBSD, Solaris 11 OmniOS.  */
  {
    struct tm *t = getdate ("Jan Wed 1989");
    ASSERT (t != NULL);
    ASSERT (t->tm_year == 1989 - 1900
            && t->tm_mon == 1 - 1
            && t->tm_mday >= 1 && t->tm_mday <= 31
            && t->tm_wday == 3
            && t->tm_hour >= 0 && t->tm_hour < 24
            && t->tm_min >= 0 && t->tm_min < 60
            && t->tm_sec >= 0 && t->tm_sec < 60);
  }

  /* This test case originally failed on NetBSD, Solaris 11 OmniOS.  */
  {
    struct tm *t = getdate ("Fri 9");
    ASSERT (t != NULL);
    ASSERT (t->tm_year >= 1970 - 1900
            && t->tm_mon >= 0 && t->tm_mon < 12
            && t->tm_mday >= 1 && t->tm_mday <= 31
            && t->tm_wday == 5
            && t->tm_hour == 9 && t->tm_min == 0 && t->tm_sec == 0);
  }

  /* This test case originally failed on NetBSD, Solaris 11 OmniOS.  */
  {
    struct tm *t = getdate ("Feb 10:30");
    ASSERT (t != NULL);
    ASSERT (t->tm_year >= 1970 - 1900
            && t->tm_mon == 2 - 1
            && t->tm_mday >= 1 && t->tm_mday <= 31
            && t->tm_hour == 10 && t->tm_min == 0 && t->tm_sec == 30);
  }

  /* This test case originally failed on NetBSD, AIX, Solaris 11 OmniOS.  */
  {
    struct tm *t = getdate ("10:30");
    ASSERT (t != NULL);
    ASSERT (t->tm_year >= 1970 - 1900
            && t->tm_mon >= 0 && t->tm_mon < 12
            && t->tm_mday >= 1 && t->tm_mday <= 31
            && t->tm_hour == 10 && t->tm_min == 30 && t->tm_sec == 0);
  }

  /* This test case originally failed on musl libc, AIX, Solaris 11 OmniOS.  */
  {
    struct tm *t = getdate ("13:30");
    ASSERT (t != NULL);
    ASSERT (t->tm_year >= 1970 - 1900
            && t->tm_mon >= 0 && t->tm_mon < 12
            && t->tm_mday >= 1 && t->tm_mday <= 31
            && t->tm_hour == 13 && t->tm_min == 30 && t->tm_sec == 0);
  }

  return test_exit_status;
}
