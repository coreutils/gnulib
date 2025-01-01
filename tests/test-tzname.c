/* Test of tzname array.
   Copyright (C) 2024-2025 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2024.  */

#include <config.h>

/* Specification.  */
#include <time.h>

#include <stdio.h>
#include <stdlib.h>

int
main ()
{
  const char *tz = getenv ("TZ");
  tzset ();
  printf ("TZ=%s -> tzname[0]=\"%s\", tzname[1]=\"%s\"\n",
          tz != NULL ? tz : "(null)",
          tzname[0], tzname[1]);
  return 0;
}
