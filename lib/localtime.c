/* Work around platform bugs in localtime.
   Copyright (C) 2017 Free Software Foundation, Inc.

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

#include <config.h>

/* Specification.  */
#include <time.h>

/* Keep consistent with gettimeofday.c!  */
#if !(GETTIMEOFDAY_CLOBBERS_LOCALTIME || TZSET_CLOBBERS_LOCALTIME)

# include <stdlib.h>
# include <string.h>

# undef localtime

struct tm *
rpl_localtime (const time_t *tp)
{
# if (defined _WIN32 || defined __WIN32__) && ! defined __CYGWIN__
  /* If the environment variable TZ has been set by Cygwin, neutralize it.
     The Microsoft CRT interprets TZ differently than Cygwin and produces
     incorrect results if TZ has the syntax used by Cygwin.  */
  const char *tz = getenv ("TZ");
  if (tz != NULL && strchr (tz, '/') != NULL)
    _putenv ("TZ=");
# endif

  return localtime (tp);
}

#endif
