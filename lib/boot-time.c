/* Determine the time when the machine last booted.
   Copyright (C) 2023 Free Software Foundation, Inc.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published
   by the Free Software Foundation, either version 3 of the License,
   or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Written by Bruno Haible <bruno@clisp.org>.  */

#include <config.h>

/* Specification.  */
#include "boot-time.h"

#include <stddef.h>

#include "idx.h"
#include "readutmp.h"

int
get_boot_time (struct timespec *p_boot_time)
{
  idx_t n_entries = 0;
  STRUCT_UTMP *utmp = NULL;
  read_utmp (UTMP_FILE, &n_entries, &utmp, READ_UTMP_BOOT_TIME);
  if (n_entries > 0)
    {
      *p_boot_time = utmp[0].ut_ts;
      free (utmp);
      return 0;
    }
  free (utmp);
  return -1;
}
