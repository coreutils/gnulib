/* gethrxtime -- get high resolution real time

   Copyright (C) 2005-2007, 2009-2026 Free Software Foundation, Inc.

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

/* Written by Paul Eggert.  */

#define GETHRXTIME_INLINE _GL_EXTERN_INLINE
#include <config.h>
#include "gethrxtime.h"

#if ! (HAVE_ARITHMETIC_HRTIME_T && HAVE_DECL_GETHRTIME)

#include <sys/time.h>
#include "timespec.h"

/* Get the current time, as a count of the number of nanoseconds since
   an arbitrary epoch (e.g., the system boot time).  Prefer a
   high-resolution clock that is not subject to resetting or
   drifting.  */

xtime_t
gethrxtime (void)
{
  struct timespec ts;
# if defined CLOCK_MONOTONIC && HAVE_CLOCK_GETTIME
  /* If available, use a monotonically increasing clock.
     Otherwise fall back on CLOCK_REALTIME as that is
     what current_timespec would do anyway.  */
  if (clock_gettime (CLOCK_MONOTONIC, &ts) < 0)
    clock_gettime (CLOCK_REALTIME, &ts);
# else
  ts = current_timespec ();
# endif
  return xtime_make (ts.tv_sec, ts.tv_nsec);
}

#endif
