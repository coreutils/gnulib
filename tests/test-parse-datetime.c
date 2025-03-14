/* Test of parse_datetime() function.
   Copyright (C) 2008-2025 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, see <https://www.gnu.org/licenses/>.  */

/* Written by Simon Josefsson <simon@josefsson.org>, 2008.  */

#include <config.h>

#include "parse-datetime.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "macros.h"

#ifdef DEBUG
#define LOG(str, now, res)                                              \
  printf ("string '%s' diff %d %d\n",                                 \
          str, res.tv_sec - now.tv_sec, res.tv_nsec - now.tv_nsec);
#else
#define LOG(str, now, res) (void) 0
#endif

static const char *const day_table[] =
{
  "SUNDAY",
  "MONDAY",
  "TUESDAY",
  "WEDNESDAY",
  "THURSDAY",
  "FRIDAY",
  "SATURDAY",
  NULL
};


#if ! HAVE_STRUCT_TM_TM_GMTOFF
/* Shift A right by B bits portably, by dividing A by 2**B and
   truncating towards minus infinity.  A and B should be free of side
   effects, and B should be in the range 0 <= B <= INT_BITS - 2, where
   INT_BITS is the number of useful bits in an int.  GNU code can
   assume that INT_BITS is at least 32.

   ISO C99 says that A >> B is implementation-defined if A < 0.  Some
   implementations (e.g., UNICOS 9.0 on a Cray Y-MP EL) don't shift
   right in the usual way when A < 0, so SHR falls back on division if
   ordinary A >> B doesn't seem to be the usual signed shift.  */
#define SHR(a, b)       \
  (-1 >> 1 == -1        \
   ? (a) >> (b)         \
   : (a) / (1 << (b)) - ((a) % (1 << (b)) < 0))

#define TM_YEAR_BASE 1900

/* Yield the difference between *A and *B,
   measured in seconds, ignoring leap seconds.
   The body of this function is taken directly from the GNU C Library;
   see src/strftime.c.  */
static long int
tm_diff (struct tm const *a, struct tm const *b)
{
  /* Compute intervening leap days correctly even if year is negative.
     Take care to avoid int overflow in leap day calculations.  */
  int a4 = SHR (a->tm_year, 2) + SHR (TM_YEAR_BASE, 2) - ! (a->tm_year & 3);
  int b4 = SHR (b->tm_year, 2) + SHR (TM_YEAR_BASE, 2) - ! (b->tm_year & 3);
  int a100 = a4 / 25 - (a4 % 25 < 0);
  int b100 = b4 / 25 - (b4 % 25 < 0);
  int a400 = SHR (a100, 2);
  int b400 = SHR (b100, 2);
  int intervening_leap_days = (a4 - b4) - (a100 - b100) + (a400 - b400);
  long int ayear = a->tm_year;
  long int years = ayear - b->tm_year;
  long int days = (365 * years + intervening_leap_days
                   + (a->tm_yday - b->tm_yday));
  return (60 * (60 * (24 * days + (a->tm_hour - b->tm_hour))
                + (a->tm_min - b->tm_min))
          + (a->tm_sec - b->tm_sec));
}
#endif

static long
gmt_offset (time_t s)
{
  long gmtoff;

#if !HAVE_STRUCT_TM_TM_GMTOFF
  struct tm tm_local = *localtime (&s);
  struct tm tm_gmt   = *gmtime (&s);

  gmtoff = tm_diff (&tm_local, &tm_gmt);
#else
  gmtoff = localtime (&s)->tm_gmtoff;
#endif

  return gmtoff;
}

/* Define SOME_TIMEPOINT to some tv_sec value that is supported by the
   platform's localtime() function and that is on the same weekday as
   the Unix epoch.  */
#if defined _WIN32 && !defined __CYGWIN__
/* On native Windows, localtime() fails for all time_t values < 0.  */
# define SOME_TIMEPOINT (700 * 86400)
#else
/* The Unix epoch.  */
# define SOME_TIMEPOINT 0
#endif

int
main (_GL_UNUSED int argc, char **argv)
{
  struct timespec result;
  struct timespec result2;
  struct timespec expected;
  struct timespec now;
  const char *p;
  int i;
  long gmtoff;
  time_t ref_time = 1304250918;

  /* Set the time zone to US Eastern time with the 2012 rules.  This
     should disable any leap second support.  Otherwise, there will be
     a problem with glibc on sites that default to leap seconds; see
     <https://bugs.gnu.org/12206>.  */
  ASSERT (setenv ("TZ", "EST5EDT,M3.2.0,M11.1.0", 1) == 0);
  tzset ();

  gmtoff = gmt_offset (ref_time);


  /* ISO 8601 extended date and time of day representation,
     'T' separator, local time zone */
  p = "2011-05-01T11:55:18";
  expected.tv_sec = ref_time - gmtoff;
  expected.tv_nsec = 0;
  ASSERT (parse_datetime (&result, p, NULL));
  LOG (p, expected, result);
  ASSERT (expected.tv_sec == result.tv_sec
          && expected.tv_nsec == result.tv_nsec);

  /* ISO 8601 extended date and time of day representation,
     ' ' separator, local time zone */
  p = "2011-05-01 11:55:18";
  expected.tv_sec = ref_time - gmtoff;
  expected.tv_nsec = 0;
  ASSERT (parse_datetime (&result, p, NULL));
  LOG (p, expected, result);
  ASSERT (expected.tv_sec == result.tv_sec
          && expected.tv_nsec == result.tv_nsec);

  /* ISO 8601 extended date and time of day representation,
     ' ' separator, 'J' (local) time zone */
  p = "2011-05-01 11:55:18J";
  expected.tv_sec = ref_time - gmtoff;
  expected.tv_nsec = 0;
  ASSERT (parse_datetime (&result, p, NULL));
  LOG (p, expected, result);
  ASSERT (expected.tv_sec == result.tv_sec
          && expected.tv_nsec == result.tv_nsec);


  /* ISO 8601, extended date and time of day representation,
     'T' separator, UTC */
  p = "2011-05-01T11:55:18Z";
  expected.tv_sec = ref_time;
  expected.tv_nsec = 0;
  ASSERT (parse_datetime (&result, p, NULL));
  LOG (p, expected, result);
  ASSERT (expected.tv_sec == result.tv_sec
          && expected.tv_nsec == result.tv_nsec);

  /* ISO 8601, extended date and time of day representation,
     ' ' separator, UTC */
  p = "2011-05-01 11:55:18Z";
  expected.tv_sec = ref_time;
  expected.tv_nsec = 0;
  ASSERT (parse_datetime (&result, p, NULL));
  LOG (p, expected, result);
  ASSERT (expected.tv_sec == result.tv_sec
          && expected.tv_nsec == result.tv_nsec);


  /* ISO 8601 extended date and time of day representation,
     'T' separator, w/UTC offset */
  p = "2011-05-01T11:55:18-07:00";
  expected.tv_sec = 1304276118;
  expected.tv_nsec = 0;
  ASSERT (parse_datetime (&result, p, NULL));
  LOG (p, expected, result);
  ASSERT (expected.tv_sec == result.tv_sec
          && expected.tv_nsec == result.tv_nsec);

  /* ISO 8601 extended date and time of day representation,
     ' ' separator, w/UTC offset */
  p = "2011-05-01 11:55:18-07:00";
  expected.tv_sec = 1304276118;
  expected.tv_nsec = 0;
  ASSERT (parse_datetime (&result, p, NULL));
  LOG (p, expected, result);
  ASSERT (expected.tv_sec == result.tv_sec
          && expected.tv_nsec == result.tv_nsec);


  /* ISO 8601 extended date and time of day representation,
     'T' separator, w/hour only UTC offset */
  p = "2011-05-01T11:55:18-07";
  expected.tv_sec = 1304276118;
  expected.tv_nsec = 0;
  ASSERT (parse_datetime (&result, p, NULL));
  LOG (p, expected, result);
  ASSERT (expected.tv_sec == result.tv_sec
          && expected.tv_nsec == result.tv_nsec);

  /* ISO 8601 extended date and time of day representation,
     ' ' separator, w/hour only UTC offset */
  p = "2011-05-01 11:55:18-07";
  expected.tv_sec = 1304276118;
  expected.tv_nsec = 0;
  ASSERT (parse_datetime (&result, p, NULL));
  LOG (p, expected, result);
  ASSERT (expected.tv_sec == result.tv_sec
          && expected.tv_nsec == result.tv_nsec);


  now.tv_sec = SOME_TIMEPOINT + 4711;
  now.tv_nsec = 1267;
  p = "now";
  ASSERT (parse_datetime (&result, p, &now));
  LOG (p, now, result);
  ASSERT (now.tv_sec == result.tv_sec && now.tv_nsec == result.tv_nsec);

  now.tv_sec = SOME_TIMEPOINT + 4711;
  now.tv_nsec = 1267;
  p = "tomorrow";
  ASSERT (parse_datetime (&result, p, &now));
  LOG (p, now, result);
  ASSERT (now.tv_sec + 24 * 60 * 60 == result.tv_sec
          && now.tv_nsec == result.tv_nsec);

  now.tv_sec = SOME_TIMEPOINT + 4711;
  now.tv_nsec = 1267;
  p = "yesterday";
  ASSERT (parse_datetime (&result, p, &now));
  LOG (p, now, result);
  ASSERT (now.tv_sec - 24 * 60 * 60 == result.tv_sec
          && now.tv_nsec == result.tv_nsec);

  now.tv_sec = SOME_TIMEPOINT + 4711;
  now.tv_nsec = 1267;
  p = "4 hours";
  ASSERT (parse_datetime (&result, p, &now));
  LOG (p, now, result);
  ASSERT (now.tv_sec + 4 * 60 * 60 == result.tv_sec
          && now.tv_nsec == result.tv_nsec);

  /* test if timezone is not being ignored for day offset */
  now.tv_sec = SOME_TIMEPOINT + 4711;
  now.tv_nsec = 1267;
  p = "UTC+400 +24 hours";
  ASSERT (parse_datetime (&result, p, &now));
  LOG (p, now, result);
  p = "UTC+400 +1 day";
  ASSERT (parse_datetime (&result2, p, &now));
  LOG (p, now, result2);
  ASSERT (result.tv_sec == result2.tv_sec
          && result.tv_nsec == result2.tv_nsec);

  /* test if several time zones formats are handled same way */
  now.tv_sec = SOME_TIMEPOINT + 4711;
  now.tv_nsec = 1267;
  p = "UTC+14:00";
  ASSERT (parse_datetime (&result, p, &now));
  LOG (p, now, result);
  p = "UTC+14";
  ASSERT (parse_datetime (&result2, p, &now));
  LOG (p, now, result2);
  ASSERT (result.tv_sec == result2.tv_sec
          && result.tv_nsec == result2.tv_nsec);
  p = "UTC+1400";
  ASSERT (parse_datetime (&result2, p, &now));
  LOG (p, now, result2);
  ASSERT (result.tv_sec == result2.tv_sec
          && result.tv_nsec == result2.tv_nsec);

  now.tv_sec = SOME_TIMEPOINT + 4711;
  now.tv_nsec = 1267;
  p = "UTC-14:00";
  ASSERT (parse_datetime (&result, p, &now));
  LOG (p, now, result);
  p = "UTC-14";
  ASSERT (parse_datetime (&result2, p, &now));
  LOG (p, now, result2);
  ASSERT (result.tv_sec == result2.tv_sec
          && result.tv_nsec == result2.tv_nsec);
  p = "UTC-1400";
  ASSERT (parse_datetime (&result2, p, &now));
  LOG (p, now, result2);
  ASSERT (result.tv_sec == result2.tv_sec
          && result.tv_nsec == result2.tv_nsec);

  now.tv_sec = SOME_TIMEPOINT + 4711;
  now.tv_nsec = 1267;
  p = "UTC+0:15";
  ASSERT (parse_datetime (&result, p, &now));
  LOG (p, now, result);
  p = "UTC+0015";
  ASSERT (parse_datetime (&result2, p, &now));
  LOG (p, now, result2);
  ASSERT (result.tv_sec == result2.tv_sec
          && result.tv_nsec == result2.tv_nsec);

  now.tv_sec = SOME_TIMEPOINT + 4711;
  now.tv_nsec = 1267;
  p = "UTC-1:30";
  ASSERT (parse_datetime (&result, p, &now));
  LOG (p, now, result);
  p = "UTC-130";
  ASSERT (parse_datetime (&result2, p, &now));
  LOG (p, now, result2);
  ASSERT (result.tv_sec == result2.tv_sec
          && result.tv_nsec == result2.tv_nsec);


  /* TZ out of range should cause parse_datetime failure */
  now.tv_sec = SOME_TIMEPOINT + 4711;
  now.tv_nsec = 1267;
  p = "UTC+25:00";
  ASSERT (!parse_datetime (&result, p, &now));

  /* Check for several invalid countable dayshifts */
  now.tv_sec = SOME_TIMEPOINT + 4711;
  now.tv_nsec = 1267;
  p = "UTC+4:00 +40 yesterday";
  ASSERT (!parse_datetime (&result, p, &now));
  p = "UTC+4:00 next yesterday";
  ASSERT (!parse_datetime (&result, p, &now));
  p = "UTC+4:00 tomorrow ago";
  ASSERT (!parse_datetime (&result, p, &now));
  p = "UTC+4:00 tomorrow hence";
  ASSERT (!parse_datetime (&result, p, &now));
  p = "UTC+4:00 40 now ago";
  ASSERT (!parse_datetime (&result, p, &now));
  p = "UTC+4:00 last tomorrow";
  ASSERT (!parse_datetime (&result, p, &now));
  p = "UTC+4:00 -4 today";
  ASSERT (!parse_datetime (&result, p, &now));

  /* And check correct usage of dayshifts */
  now.tv_sec = SOME_TIMEPOINT + 4711;
  now.tv_nsec = 1267;
  p = "UTC+400 tomorrow";
  ASSERT (parse_datetime (&result, p, &now));
  LOG (p, now, result);
  p = "UTC+400 +1 day";
  ASSERT (parse_datetime (&result2, p, &now));
  LOG (p, now, result2);
  ASSERT (result.tv_sec == result2.tv_sec
          && result.tv_nsec == result2.tv_nsec);
  p = "UTC+400 1 day hence";
  ASSERT (parse_datetime (&result2, p, &now));
  LOG (p, now, result2);
  ASSERT (result.tv_sec == result2.tv_sec
          && result.tv_nsec == result2.tv_nsec);
  now.tv_sec = SOME_TIMEPOINT + 4711;
  now.tv_nsec = 1267;
  p = "UTC+400 yesterday";
  ASSERT (parse_datetime (&result, p, &now));
  LOG (p, now, result);
  p = "UTC+400 1 day ago";
  ASSERT (parse_datetime (&result2, p, &now));
  LOG (p, now, result2);
  ASSERT (result.tv_sec == result2.tv_sec
          && result.tv_nsec == result2.tv_nsec);
  now.tv_sec = SOME_TIMEPOINT + 4711;
  now.tv_nsec = 1267;
  p = "UTC+400 now";
  ASSERT (parse_datetime (&result, p, &now));
  LOG (p, now, result);
  p = "UTC+400 +0 minutes"; /* silly, but simple "UTC+400" is different*/
  ASSERT (parse_datetime (&result2, p, &now));
  LOG (p, now, result2);
  ASSERT (result.tv_sec == result2.tv_sec
          && result.tv_nsec == result2.tv_nsec);

  /* If this platform has TZDB, check for GNU Bug#48085.  */
#if defined _WIN32 && !defined __CYGWIN__
  ASSERT (setenv ("TZ", "US Eastern Standard Time", 1) == 0);
#else
  ASSERT (setenv ("TZ", "America/Indiana/Indianapolis", 1) == 0);
#endif
  tzset ();
  now.tv_sec = 1619641490;
  now.tv_nsec = 0;
  struct tm *tm = localtime (&now.tv_sec);
  if (tm && tm->tm_year == 2021 - 1900 && tm->tm_mon == 4 - 1
      && tm->tm_mday == 28 && tm->tm_hour == 16 && tm->tm_min == 24
      && 0 < tm->tm_isdst)
    {
      int has_leap_seconds = tm->tm_sec != now.tv_sec % 60;
      p = "now - 35 years";
      ASSERT (parse_datetime (&result, p, &now));
      LOG (p, now, result);
      /* Allow "now - 35 years" to mean either (a) the naive sense
         where subtracting 35 years from 2021-04-28 16:24:50 yields
         1986-04-28 16:24:50, or (b) the pedantic sense where it
         yields 1986-04-28 15:24:50 (an hour earlier) due to adjusting
         for the switch from DST in 2021 to standard time in 1986.  */
      time_t naive = 515107490 + (has_leap_seconds ? 13 : 0);
      time_t pedantic = naive - 60 * 60;
      ASSERT (result.tv_sec == naive || result.tv_sec == pedantic);
    }

  /* Check that some "next Monday", "last Wednesday", etc. are correct.  */
  ASSERT (setenv ("TZ", "UTC0", 1) == 0);
  tzset ();
  for (i = 0; day_table[i]; i++)
    {
      unsigned int thur2 = SOME_TIMEPOINT + 7 * 24 * 3600; /* 2nd thursday */
      char tmp[32];
      sprintf (tmp, "NEXT %s", day_table[i]);
      now.tv_sec = thur2 + 4711;
      now.tv_nsec = 1267;
      ASSERT (parse_datetime (&result, tmp, &now));
      LOG (tmp, now, result);
      ASSERT (result.tv_nsec == 0);
      ASSERT (result.tv_sec == thur2 + (i == 4 ? 7 : (i + 3) % 7) * 24 * 3600);

      sprintf (tmp, "LAST %s", day_table[i]);
      now.tv_sec = thur2 + 4711;
      now.tv_nsec = 1267;
      ASSERT (parse_datetime (&result, tmp, &now));
      LOG (tmp, now, result);
      ASSERT (result.tv_nsec == 0);
      ASSERT (result.tv_sec == thur2 + ((i + 3) % 7 - 7) * 24 * 3600);
    }

/* On native Windows, localtime() fails for all time_t values < 0.  */
#if !(defined _WIN32 && !defined __CYGWIN__)
  p = "1970-12-31T23:59:59+00:00 - 1 year";  /* Bug#50115 */
  now.tv_sec = -1;
  now.tv_nsec = 0;
  ASSERT (parse_datetime (&result, p, &now));
  LOG (p, now, result);
  ASSERT (result.tv_sec == now.tv_sec
          && result.tv_nsec == now.tv_nsec);

  p = "THURSDAY UTC+00";  /* The epoch was on Thursday.  */
  now.tv_sec = 0;
  now.tv_nsec = 0;
  ASSERT (parse_datetime (&result, p, &now));
  LOG (p, now, result);
  ASSERT (result.tv_sec == now.tv_sec
          && result.tv_nsec == now.tv_nsec);

  p = "FRIDAY UTC+00";
  now.tv_sec = 0;
  now.tv_nsec = 0;
  ASSERT (parse_datetime (&result, p, &now));
  LOG (p, now, result);
  ASSERT (result.tv_sec == 24 * 3600
          && result.tv_nsec == now.tv_nsec);
#endif

  /* Exercise a sign-extension bug.  Before July 2012, an input
     starting with a high-bit-set byte would be treated like "0".  */
  ASSERT ( ! parse_datetime (&result, "\xb0", &now));

  /* Exercise TZ="" parsing code.  */
  /* These two would infloop or segfault before Feb 2014.  */
  ASSERT ( ! parse_datetime (&result, "TZ=\"\"\"", &now));
  ASSERT ( ! parse_datetime (&result, "TZ=\"\" \"", &now));
  /* Exercise invalid patterns.  */
  ASSERT ( ! parse_datetime (&result, "TZ=\"", &now));
  ASSERT ( ! parse_datetime (&result, "TZ=\"\\\"", &now));
  ASSERT ( ! parse_datetime (&result, "TZ=\"\\n", &now));
  ASSERT ( ! parse_datetime (&result, "TZ=\"\\n\"", &now));
  /* Exercise valid patterns.  */
  ASSERT (   parse_datetime (&result, "TZ=\"\"", &now));
  ASSERT (   parse_datetime (&result, "TZ=\"\" ", &now));
  ASSERT (   parse_datetime (&result, " TZ=\"\"", &now));
  /* Exercise patterns which may be valid or invalid, depending on the
     platform.  */
#if !defined __NetBSD__
  ASSERT (   parse_datetime (&result, "TZ=\"\\\\\"", &now));
  ASSERT (   parse_datetime (&result, "TZ=\"\\\"\"", &now));
#endif

  /* Outlandishly-long time zone abbreviations should not cause problems.  */
  {
    static char const bufprefix[] = "TZ=\"";
    long int tzname_max = -1;
    errno = 0;
#ifdef _SC_TZNAME_MAX
    tzname_max = sysconf (_SC_TZNAME_MAX);
#endif
    enum { tzname_alloc = 2000 };
    if (tzname_max < 0)
      tzname_max = errno ? 6 : tzname_alloc;
    int tzname_len = tzname_alloc < tzname_max ? tzname_alloc : tzname_max;
    static char const bufsuffix[] = "0\" 1970-01-01 01:02:03.123456789";
    enum { bufsize = sizeof bufprefix - 1 + tzname_alloc + sizeof bufsuffix };
    char buf[bufsize];
    memcpy (buf, bufprefix, sizeof bufprefix - 1);
    memset (buf + sizeof bufprefix - 1, 'X', tzname_len);
    strcpy (buf + sizeof bufprefix - 1 + tzname_len, bufsuffix);
    ASSERT (parse_datetime (&result, buf, &now));
    LOG (buf, now, result);
    ASSERT (result.tv_sec == 1 * 60 * 60 + 2 * 60 + 3
            && result.tv_nsec == 123456789);
  }

  return test_exit_status;
}
