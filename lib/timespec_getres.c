/* Get resolution of a time base.
   Copyright 2021-2022 Free Software Foundation, Inc.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#include <config.h>

#include <time.h>

#include "timespec.h"

/* Set TS to resolution of time base BASE.
   Return BASE if successful, 0 otherwise.  */
int
timespec_getres (struct timespec *ts, int base)
{
  if (base == TIME_UTC)
    {
#if defined CLOCK_REALTIME && HAVE_CLOCK_GETRES
      clock_getres (CLOCK_REALTIME, ts);
#else
      long int res = gettime_res ();
      ts->tv_sec = res / TIMESPEC_HZ;
      ts->tv_nsec = res % TIMESPEC_HZ;
#endif
      return base;
    }
  return 0;
}
