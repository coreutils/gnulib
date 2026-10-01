/* Test localtime_r() on native Windows.
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
     by Cygwin but not by native Windows, the localtime_r function is still
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
  int hours_since_1970_01_01 = 497000; /* 2026-09-12 08:00:00 UTC */
  time_t seconds_since_1970_01_01 = hours_since_1970_01_01 * 3600;

  /* Convert it to local time through the Windows API functions.  */
  long long hundrednanos_since_1601_01_01 =
    (seconds_since_1970_01_01 + 134774LL * 24LL * 3600LL) * 10000000LL;
  FILETIME filetime =
    { (DWORD) hundrednanos_since_1601_01_01,
      (DWORD) (hundrednanos_since_1601_01_01 >> 32)
    };

  /* Convert like GNU clisp does.  */
  SYSTEMTIME lsystime1;
  {
    FILETIME lfiletime;
    /* Documentation:
       <https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-filetimetolocalfiletime>  */
    if (!FileTimeToLocalFileTime (&filetime, &lfiletime))
      return 3;
    /* Documentation:
       <https://learn.microsoft.com/en-us/windows/win32/api/timezoneapi/nf-timezoneapi-filetimetosystemtime>  */
    if (!FileTimeToSystemTime (&lfiletime, &lsystime1))
      return 4;
  }
  printf ("Windows API 1: %04d-%02d-%02d %02d:%02d:%02d\n",
          lsystime1.wYear, lsystime1.wMonth, lsystime1.wDay,
          lsystime1.wHour, lsystime1.wMinute, lsystime1.wSecond);

  /* Convert like
     <https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-filetimetolocalfiletime>
     suggests.  */
  SYSTEMTIME lsystime2;
  {
    SYSTEMTIME systime;
    /* Documentation:
       <https://learn.microsoft.com/en-us/windows/win32/api/timezoneapi/nf-timezoneapi-filetimetosystemtime>  */
    if (!FileTimeToSystemTime (&filetime, &systime))
      return 5;
    /* Documentation:
       <https://learn.microsoft.com/en-us/windows/win32/api/timezoneapi/nf-timezoneapi-systemtimetotzspecificlocaltime>  */
    if (!SystemTimeToTzSpecificLocalTime (NULL, &systime, &lsystime2))
      return 6;
  }
  printf ("Windows API 2: %04d-%02d-%02d %02d:%02d:%02d\n",
          lsystime2.wYear, lsystime2.wMonth, lsystime2.wDay,
          lsystime2.wHour, lsystime2.wMinute, lsystime2.wSecond);

  /* Both methods should be equivalent.  */
  if (lsystime1.wHour != lsystime2.wHour)
    return 7;

  /* Now see whether localtime_r is consistent with that.  */
  struct tm lm;
  if (localtime_r (&seconds_since_1970_01_01, &lm) == NULL)
    return 8;
  printf ("localtime_r:   %04d-%02d-%02d %02d:%02d:%02d (DST=%d)\n",
          lm.tm_year + 1900, lm.tm_mon + 1, lm.tm_mday,
          lm.tm_hour, lm.tm_min, lm.tm_sec,
          !!lm.tm_isdst);
  fflush (stdout);
  ASSERT (lm.tm_year + 1900 == lsystime1.wYear);
  ASSERT (lm.tm_mon + 1 == lsystime1.wMonth);
  ASSERT (lm.tm_mday == lsystime1.wDay);
  ASSERT (lm.tm_hour == lsystime1.wHour);
  ASSERT (lm.tm_min == lsystime1.wMinute);
  ASSERT (lm.tm_sec == lsystime1.wSecond);

  return test_exit_status;
}
