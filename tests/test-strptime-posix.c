/* Test of conversion of a time string using a complete format string.
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

#include "signature.h"
SIGNATURE_CHECK (strptime, char *, (const char *, const char *, struct tm *));

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
   And then:
     "Conforming applications should make a single call to strptime() with a
      format and all data needed to completely specify the date and time being
      converted."
   This is what we test here.  */

int
main ()
{
  struct tm t;
  memset (&t, '\0', sizeof (t));
  t.tm_isdst = -1;
  const char *end =
    strptime ("Friday September 18, 1987, 10:29:30", "%A %B %d, %Y, %H:%M:%S",
              &t);
  ASSERT (end != NULL && *end == '\0');
  ASSERT (t.tm_year == 1987 - 1900
          && t.tm_mon == 9 - 1
          && t.tm_mday == 18
          && t.tm_hour == 10 && t.tm_min == 29 && t.tm_sec == 30);

  return test_exit_status;
}
