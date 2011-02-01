/* Test of <time.h> substitute in C++ mode.
   Copyright (C) 2010-2011 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* Written by Bruno Haible <bruno@clisp.org>, 2010.  */

#define GNULIB_NAMESPACE gnulib
#include <config.h>

#include <time.h>

#include "signature.h"


#if GNULIB_TEST_NANOSLEEP
SIGNATURE_CHECK (GNULIB_NAMESPACE::nanosleep, int,
                 (struct timespec const *, struct timespec *));
#endif

#if GNULIB_TEST_MKTIME
SIGNATURE_CHECK (GNULIB_NAMESPACE::mktime, time_t, (struct tm *));
#endif

#if GNULIB_TEST_TIME_R
SIGNATURE_CHECK (GNULIB_NAMESPACE::localtime_r, struct tm *,
                 (time_t const *, struct tm *));

SIGNATURE_CHECK (GNULIB_NAMESPACE::gmtime_r, struct tm *,
                 (time_t const *, struct tm *));
#endif

#if GNULIB_TEST_STRPTIME
SIGNATURE_CHECK (GNULIB_NAMESPACE::strptime, char *,
                 (char const *, char const *, struct tm *));
#endif

#if GNULIB_TEST_TIMEGM
SIGNATURE_CHECK (GNULIB_NAMESPACE::timegm, time_t, (struct tm *));
#endif


int
main ()
{
}
