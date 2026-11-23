/* Test of conversion of a time string using an incomplete format string.
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

#include <string.h>

#include "macros.h"

/* POSIX
   <https://pubs.opengroup.org/onlinepubs/9799919799/functions/strptime.html>
   says:
     "It is unspecified whether multiple calls to strptime() using the same tm
      structure will update the current contents of the structure or overwrite
      all contents of the structure."
   All implementations except Solaris update the contents of the structure.
   Only Solaris overwrites all contents of the structure.
   Here we test for the "update the contents" behaviour.  */

int
main ()
{
  {
    struct tm t;
    memset (&t, '\0', sizeof (t));
    t.tm_year = -9999; t.tm_mon = -9999; t.tm_mday = -9999;
    t.tm_hour = -9999; t.tm_min = -9999; t.tm_sec = -9999;
    t.tm_isdst = -1;
    const char *end = strptime ("Friday September 18, 1987", "%A %B %d, %Y", &t);
    ASSERT (end != NULL && *end == '\0');
    ASSERT (t.tm_hour == -9999 && t.tm_min == -9999 && t.tm_sec == -9999);
  }
  {
    struct tm t;
    memset (&t, '\0', sizeof (t));
    t.tm_year = -9999; t.tm_mon = -9999; t.tm_mday = -9999;
    t.tm_hour = -9999; t.tm_min = -9999; t.tm_sec = -9999;
    t.tm_isdst = -1;
    const char *end = strptime ("10:29:30", "%H:%M:%S", &t);
    ASSERT (end != NULL && *end == '\0');
    ASSERT (t.tm_year == -9999 && t.tm_mon == -9999 && t.tm_mday == -9999);
  }

  return test_exit_status;
}
