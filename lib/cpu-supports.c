/* Support routines to query GLIBC_TUNABLES=glibc.cpu.hwcaps

   Copyright 2025 Free Software Foundation, Inc.

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

#include <config.h>
#include <stdlib.h>
#include <string.h>

#include "cpu-supports.h"

/* Allow reparsing of env var for testing.  */
bool hwcap_allowed_nocache = false;

extern bool
hwcap_allowed (char const *glibc_hwcap)
{
  if (! glibc_hwcap)
    return true;

  if (*glibc_hwcap != '-')  /* Need to pass HWCAP with a leading -  */
    return true;

  /* Match how GLIBC parses tunables as indicated with:
       GLIBC_TUNABLES=glibc.cpu.hwcaps=... ld.so --list-tunables | grep hwcaps

     GLIBC_TUNABLES items are delimited with ':',
     and like glibc we take the last instance of glibc.cpu.hwcaps.

     An example of the usual format is:
       GLIBC_TUNABLES=glibc.cpu.hwcaps=-AVX,-AVX2  */

  static char const *hwcaps;
  if (hwcap_allowed_nocache || ! hwcaps)
    { /* Cache glibc.cpu.hwcaps once per process.  */
      if ((hwcaps = getenv ("GLIBC_TUNABLES")))
        {
          char const *tunables_start = hwcaps;
          char const *last_hwcaps;
          while ((last_hwcaps = strstr (hwcaps, "glibc.cpu.hwcaps=")))
            hwcaps = last_hwcaps + sizeof "glibc.cpu.hwcaps=" - 1;
          if (hwcaps == tunables_start)  /* No match.  */
            hwcaps = "";
        }
      else
        hwcaps = "";
    }

  if (! hwcaps)  /* Shouldn't happen.  */
    return true;

  if (! *hwcaps)
    return true;

  char const *sentinel = strchr (hwcaps, ':');
  if (! sentinel)
    sentinel = hwcaps + strlen (hwcaps);
  char const *cap = hwcaps;
  while ((cap = strstr (cap, glibc_hwcap)) && cap < sentinel)
    { /* Check it's not a partial match.  */
      cap += strlen (glibc_hwcap);
      if (*cap == ',' || *cap == ':' || *cap == '\0')
        return false;  /* Feature disabled.  */
      /* glibc hwcaps can't have '-' in name so ok to search from here. */
    }

  return true;
}
