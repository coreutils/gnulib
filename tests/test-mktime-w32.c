/* Test mktime() on native Windows.
   Copyright (C) 2026 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2026.  */

#include <config.h>

/* Specification.  */
#include <time.h>

#include <stdio.h>
#include <stdlib.h>

#define WIN32_LEAN_AND_MEAN  /* avoid including junk */
#include <windows.h>

#include "macros.h"

int
main (void)
{
  /* Check that when the TZ environment variable is set to a value understood
     by Cygwin but not by native Windows, the mktime function is still
     consistent with the Windows API functions that uses the time zone settings
     from the System Settings / Control Panel.  */

  /* Set the TZ environment variable.
     Documentation:
     <https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-setenvironmentvariable>
     <https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/putenv-wputenv>  */
  if (!SetEnvironmentVariable ("TZ", "Europe/Berlin"))
    return 2;
  if (_putenv ("TZ=Europe/Berlin"))
    return 2;

  /* Pick a specific time point, not too far away in the past.  */
  struct tm lm;
  /* 2026-09-12 08:00:00 local time */
  lm.tm_year = 2026 - 1900;
  lm.tm_mon = 9 - 1;
  lm.tm_mday = 12;
  lm.tm_hour = 8;
  lm.tm_min = 0;
  lm.tm_sec = 0;

  /* Convert it to UTC through the Windows API functions.  */
  SYSTEMTIME lsystime;
  lsystime.wYear = lm.tm_year + 1900;
  lsystime.wMonth = lm.tm_mon + 1;
  lsystime.wDay = lm.tm_mday;
  lsystime.wHour = lm.tm_hour;
  lsystime.wMinute = lm.tm_min;
  lsystime.wSecond = lm.tm_sec;
  lsystime.wMilliseconds = 0;

  /* Convert like GNU clisp does.  */
  time_t utctime1;
  {
    FILETIME lfiletime;
    FILETIME filetime;
    /* Documentation:
       <https://learn.microsoft.com/en-us/windows/win32/api/timezoneapi/nf-timezoneapi-systemtimetofiletime>  */
    if (!SystemTimeToFileTime (&lsystime, &lfiletime))
      return 3;
    /* Documentation:
       <https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-localfiletimetofiletime>  */
    if (!LocalFileTimeToFileTime (&lfiletime, &filetime))
      return 4;
    long long hundrednanos_since_1601_01_01 =
      ((unsigned long long) filetime.dwHighDateTime << 32)
      | (unsigned long long) filetime.dwLowDateTime;
    utctime1 = (hundrednanos_since_1601_01_01 / 10000000LL) - 134774LL * 24LL * 3600LL;
  }
  printf ("Windows API 1: %lld\n", (long long) utctime1);

  /* Convert like
     <https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-filetimetolocalfiletime>
     suggests.  */
  time_t utctime2;
  {
    SYSTEMTIME systime;
    FILETIME filetime;
    /* Documentation:
       <https://learn.microsoft.com/en-us/windows/win32/api/timezoneapi/nf-timezoneapi-tzspecificlocaltimetosystemtime>  */
    if (!TzSpecificLocalTimeToSystemTime (NULL, &lsystime, &systime))
      return 5;
    /* Documentation:
       <https://learn.microsoft.com/en-us/windows/win32/api/timezoneapi/nf-timezoneapi-systemtimetofiletime>  */
    if (!SystemTimeToFileTime (&systime, &filetime))
      return 6;
    long long hundrednanos_since_1601_01_01 =
      ((unsigned long long) filetime.dwHighDateTime << 32)
      | (unsigned long long) filetime.dwLowDateTime;
    utctime2 = (hundrednanos_since_1601_01_01 / 10000000LL) - 134774LL * 24LL * 3600LL;
  }
  printf ("Windows API 2: %lld\n", (long long) utctime2);

  /* Both methods should be equivalent.  */
  if (utctime1 != utctime2)
    return 7;

  /* Now see whether mktime (a.k.a. timelocal) is consistent with that.  */
  /* Documentation:
     <https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/mktime-mktime32-mktime64>  */
  time_t utctime;
  {
    struct tm tm1 = lm;
    tm1.tm_isdst = -1; /* let mktime find out whether DST or not */
    utctime = mktime (&tm1);
    if (utctime == (time_t) -1)
      return 8;
  }
  printf ("mktime:        %lld\n", (long long) utctime);
  /* Just for comparison, print the interpretation as UTC time.  */
  printf ("timegm:        %lld\n", (long long) (497000 * 3600));
  fflush (stdout);
  ASSERT (utctime == utctime1);

  return test_exit_status;
}
