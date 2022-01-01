/* pt_chown - helper program for 'grantpt'.
   Copyright (C) 1998-1999, 2009-2022 Free Software Foundation, Inc.
   Contributed by C. Scott Ananian <cananian@alumni.princeton.edu>, 1998.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of the
   License, or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#include <config.h>

#include <errno.h>
#include <grp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "root-uid.h"

#include "pty-private.h"

/* For security reasons, we try to minimize the dependencies on libraries
   outside libc.  This means, in particular:
     - No use of gettext(), since it's usually implemented in libintl.
     - No use of error() or argp, since they rely on gettext by default.  */


static int
do_pt_chown (void)
{
  char *pty;
  struct stat st;
  struct group *p;
  gid_t gid;

  /* Check that PTY_FILENO is a valid master pseudo terminal.  */
  pty = ptsname (PTY_FILENO);
  if (pty == NULL)
    return errno == EBADF ? FAIL_EBADF : FAIL_EINVAL;

  /* Check that the returned slave pseudo terminal is a
     character device.  */
  if (stat (pty, &st) < 0 || !S_ISCHR (st.st_mode))
    return FAIL_EINVAL;

  /* Get the group ID of the special 'tty' group.  */
  p = getgrnam (TTY_GROUP);
  gid = p ? p->gr_gid : getgid ();

  /* Set the owner to the real user ID, and the group to that special
     group ID.  */
  if (st.st_gid != gid && chown (pty, getuid (), gid) < 0)
    return FAIL_EACCES;

  /* Set the permission mode to readable and writable by the owner,
     and writable by the group.  */
  if ((st.st_mode & (S_IRWXU|S_IRWXG|S_IRWXO)) != (S_IRUSR|S_IWUSR|S_IWGRP)
      && chmod (pty, S_IRUSR|S_IWUSR|S_IWGRP) < 0)
    return FAIL_EACCES;

  return 0;
}


int
main (int argc, char *argv[])
{
  uid_t euid = geteuid ();

  if (argc == 1 && euid == ROOT_UID)
    {
      /* Normal invocation of this program is with no arguments and
         with privileges.  */
      return do_pt_chown ();
    }

  /* It would be possible to drop setuid/setgid privileges here.  But it is not
     really needed, since the code below only calls strcmp and [f]printf.  */

  {
    int do_help = 0;
    int do_version = 0;
    int remaining;

    for (remaining = 1; remaining < argc; remaining++)
      {
        const char *arg = argv[remaining];

        if (arg[0] == '-')
          {
            if (strcmp (arg, "--") == 0)
              {
                remaining++;
                break;
              }
            else if (strcmp (arg, "--help") == 0)
              do_help = 1;
            else if (strcmp (arg, "--version") == 0)
              do_version = 1;
            else
              {
                fprintf (stderr, "pt_chown: invalid option: %s\n", arg);
                return EXIT_FAILURE;
              }
          }
        else
          break;
      }

    if (remaining < argc)
      {
        fprintf (stderr, "pt_chown: too many arguments\n");
        return EXIT_FAILURE;
      }

    if (do_help)
      {
        printf ("Usage: pt_chown [OPTION...]\n");
        printf ("Set the owner, group and access permission of the slave pseudo terminal\n"
                "corresponding to the master pseudo terminal passed on file descriptor %d.\n"
                "This is the helper program for the 'grantpt' function.  It is not intended\n"
                "to be run directly from the command line.\n",
                PTY_FILENO);
        printf ("\n");
        printf ("  --help                     Give this help list\n");
        printf ("  --version                  Print program version\n");
        printf ("\n");
        printf ("The owner is set to the current user, the group is set to '%s', and the\n"
                "access permission is set to '%o'.\n",
                TTY_GROUP, S_IRUSR|S_IWUSR|S_IWGRP);
        printf ("Please report bugs to <bug-gnulib@gnu.org>.\n");
        return EXIT_SUCCESS;
      }

    if (do_version)
      {
        printf ("pt_chown (GNU %s) %s\n", "libc", "2.11");
        printf ("Copyright (C) %s Free Software Foundation, Inc.\n"
                "This is free software; see the source for copying conditions.  There is NO\n"
                "warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n",
                "1999");
        return EXIT_SUCCESS;
      }
  }

  /* Check if we are properly installed.  */
  if (euid != ROOT_UID)
    {
      fprintf (stderr, "pt_chown: needs to be installed setuid 'root'\n");
      return FAIL_EXEC;
    }

  return EXIT_SUCCESS;
}
