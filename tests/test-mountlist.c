/* Test the mountlist module.
   Copyright (C) 2025 Free Software Foundation, Inc.

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

/* Written by Collin Funk <collin.funk1@gmail.com>, 2025.  */

#include <config.h>

/* Specification.  */
#include "mountlist.h"

#include <stdio.h>

#include "macros.h"

int
main (void)
{
  struct mount_entry *mounts;
  struct mount_entry *p;

  mounts = read_file_system_list (false);

  /* Assume at least one mount point.  */
  ASSERT (mounts != NULL);

  for (p = mounts; p != NULL;)
    {
      struct mount_entry *next = p->me_next;
      printf ("%s %s %s %s %s\n",
              p->me_devname ? p->me_devname : "???",
              p->me_mountdir ? p->me_mountdir : "???",
              p->me_mntroot ? p->me_mntroot : "???",
              p->me_type ? p->me_type : "???",
              p->me_remote ? "remote" : "local");
      free_mount_entry (p);
      p = next;
    }

  return test_exit_status;
}
