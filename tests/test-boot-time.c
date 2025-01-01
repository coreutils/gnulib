/* Test of getting the boot time.
   Copyright (C) 2023-2025 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2023.  */

#include <config.h>

#include "boot-time.h"

#include <stddef.h>
#include <stdio.h>
#include <time.h>

#include "macros.h"

int
main (int argc, char *argv[])
{
  struct timespec boot_time;
  ASSERT (get_boot_time (&boot_time) == 0);

  time_t tim = boot_time.tv_sec;
  struct tm *gmt = gmtime (&tim);
  ASSERT (gmt != NULL);
  char timbuf[100];
  ASSERT (strftime (timbuf, sizeof (timbuf), "%Y-%m-%d %H:%M:%S", gmt) > 0);

  printf ("Boot time (UTC): %s.%09ld\n", timbuf, (long) boot_time.tv_nsec);

  /* If the boot time is more than 5 years in the past or more than a week
     in the future, the value must be wrong.  */
  time_t now = time (NULL);
  ASSERT (tim >= now - 157680000);
  ASSERT (tim <= now + 604800);

  return test_exit_status;
}
