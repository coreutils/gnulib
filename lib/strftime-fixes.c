/* Work around platform bugs in strftime.
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

#include <stdlib.h>
#include <string.h>

#undef strftime

size_t
rpl_strftime (char *buf, size_t bufsize, const char *format, const struct tm *tp)
{
#if (defined _WIN32 || defined __WIN32__) && ! defined __CYGWIN__
  /* If the environment variable TZ has been set by Cygwin, neutralize it.
     The Microsoft CRT interprets TZ differently than Cygwin and produces
     incorrect results if TZ has the syntax used by Cygwin.  */
  const char *tz = getenv ("TZ");
  if (tz != NULL && strchr (tz, '/') != NULL)
    _putenv ("TZ=");
#endif

  return strftime (buf, bufsize, format, tp);
}
