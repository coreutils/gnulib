/* Provide tzset for systems that don't have it or for which it's broken.

   Copyright (C) 2001-2003, 2005-2007, 2009-2017 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, see <http://www.gnu.org/licenses/>.  */

/* written by Jim Meyering */

#include <config.h>

/* Specification.  */
#include <time.h>

#include "localtime-buffer.h"

/* This is a wrapper for tzset, for systems on which tzset may clobber
   the static buffer used for localtime's result.
   Work around the bug in some systems whereby tzset clobbers the
   static buffer that localtime uses for its return value.  The
   tzset function from Solaris 2.5, 2.5.1, and 2.6 has this problem.  */

void
tzset (void)
#undef tzset
{
#if TZSET_CLOBBERS_LOCALTIME
  /* Save and restore the contents of the buffer used for localtime's
     result around the call to tzset.  */
  struct tm save = *localtime_buffer_addr;
#endif

#if (defined _WIN32 || defined __WIN32__) && ! defined __CYGWIN__
  /* If the environment variable TZ has been set by Cygwin, neutralize it.
     The Microsoft CRT interprets TZ differently than Cygwin and produces
     incorrect results if TZ has the syntax used by Cygwin.  */
  const char *tz = getenv ("TZ");
  if (tz != NULL && strchr (tz, '/') != NULL)
    _putenv ("TZ=");

  /* On native Windows, tzset() is deprecated.  Use _tzset() instead.  See
     https://msdn.microsoft.com/en-us/library/ms235451.aspx
     https://msdn.microsoft.com/en-us/library/90s5c885.aspx  */
  _tzset ();
#elif HAVE_TZSET
  tzset ();
#else
  /* Do nothing.  Avoid infinite recursion.  */
#endif

#if TZSET_CLOBBERS_LOCALTIME
  *localtime_buffer_addr = save;
#endif
}
