/* Test of hasmntopt() function.
   Copyright (C) 2024-2025 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2024.  */

#include <config.h>

#if HAVE_SETMNTENT

/* Specification.  */
# include <mntent.h>

# include "macros.h"

int
main ()
{
  char options[] = "rw,nosuid,nodev,noexec,relatime,size=5120k,inode64";
  struct mntent me;
  me.mnt_opts = options;

  ASSERT (hasmntopt (&me, "ro") == NULL);
  ASSERT (hasmntopt (&me, "rw") == options + 0);
  ASSERT (hasmntopt (&me, "atime") == NULL);
  ASSERT (hasmntopt (&me, "rela") == NULL);
  ASSERT (hasmntopt (&me, "relatime") == options + 23);
  ASSERT (hasmntopt (&me, "size") == options + 32);
  ASSERT (hasmntopt (&me, "size=512") == NULL);
  ASSERT (hasmntopt (&me, "size=5120k") == options + 32);
  ASSERT (hasmntopt (&me, "inode64") == options + 43);

  return test_exit_status;
}

#else

# include <stdio.h>

int
main ()
{
  fputs ("Skipping test: no setmntent, getmntent, endmntent functions\n",
         stderr);
  return 77;
}

#endif
