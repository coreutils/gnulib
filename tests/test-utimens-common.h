/* Test of file timestamp modification functions.
   Copyright (C) 2009 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* This file defines some prerequisites useful to utime-related tests.  */

#ifndef GL_TEST_UTIMENS_COMMON
# define GL_TEST_UTIMENS_COMMON

#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "stat-time.h"
#include "timespec.h"
#include "utimecmp.h"

enum {
  BILLION = 1000 * 1000 * 1000,

  Y2K = 946684800, /* Jan 1, 2000, in seconds since epoch.  */

  /* Bogus positive and negative tv_nsec values closest to valid
     range, but without colliding with UTIME_NOW or UTIME_OMIT.  */
  UTIME_BOGUS_POS = BILLION + ((UTIME_NOW == BILLION || UTIME_OMIT == BILLION)
                               ? (1 + (UTIME_NOW == BILLION + 1)
                                  + (UTIME_OMIT == BILLION + 1))
                               : 0),
  UTIME_BOGUS_NEG = -1 - ((UTIME_NOW == -1 || UTIME_OMIT == -1)
                          ? (1 + (UTIME_NOW == -2) + (UTIME_OMIT == -2))
                          : 0)
};

/* Sleep long enough to cross a timestamp quantization boundary on
   most known systems with subsecond timestamp resolution.  For
   example, ext4 has a quantization of 10 milliseconds, but a
   resolution of 1 nanosecond.  Likewise, NTFS has a quantization as
   slow as 15.25 milliseconds, but a resolution of 100 nanoseconds.
   This is necessary on systems where creat or utimens with NULL
   rounds down to the quantization boundary, but where gettime and
   hence utimensat can inject timestamps between quantization
   boundaries.  By ensuring we cross a boundary, we are less likely to
   confuse utimecmp for two times that would round to the same
   quantization boundary but are distinct based on resolution.  */
static void
nap (void)
{
  /* Systems that lack usleep also lack subsecond timestamps, and have
     a quantization boundary equal to the resolution.  Our usage of
     utimecmp allows equality, so no need to waste 980 milliseconds
     if the replacement usleep rounds to 1 second.  */
#if HAVE_USLEEP
  usleep (20 * 1000); /* 20 milliseconds.  */
#endif
}

#endif /* GL_TEST_UTIMENS_COMMON */
