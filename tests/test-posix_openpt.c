/* Test of posix_openpt function.
   Copyright (C) 2011-2025 Free Software Foundation, Inc.

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

/* Written by Eric Blake <eblake@redhat.com>, 2011.  */

#include <config.h>

#include <stdlib.h>

#include "signature.h"
SIGNATURE_CHECK (posix_openpt, int, (int));

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#if defined __sun || defined __hpux /* Solaris, HP-UX */
# include <stropts.h>
#endif

#include "macros.h"

int
main (void)
{
  int master;
  int slave;
  char *name;

  /* Open the master of a pseudo-terminal pair.  */
  master = posix_openpt (O_RDWR | O_NOCTTY);
  if (master < 0 && errno == ENOSYS)
    {
      fputs ("skipping: platform lacks pty support\n", stderr);
      return 77;
    }

  ASSERT (0 <= master);
  name = ptsname (master);
  ASSERT (name);
  ASSERT (grantpt (master) == 0);
  ASSERT (unlockpt (master) == 0);

#if defined __HAIKU__
  /* On Haiku, the open() call below succeeds only if
       - done in a child process, and
       - if that child process has done a setsid() call.
     So, do that.  */
  pid_t child_pid = fork ();
  ASSERT (child_pid >= 0);
  if (child_pid > 0)
    {
      /* We are in the parent.  Wait for the child to terminate.  */
      int child_status;
      ASSERT (waitpid (child_pid, &child_status, 0) != -1);
      ASSERT (WIFEXITED (child_status));
      int child_exit_status = WEXITSTATUS (child_status);
      ASSERT (child_exit_status == 0);

      return test_exit_status;
    }
  /* We are in the child.  */
  ASSERT (setsid () >= 0);
#endif

  slave = open (name, O_RDWR);
  ASSERT (0 <= slave);

#if defined __sun || defined __hpux /* Solaris, HP-UX */
  ASSERT (ioctl (slave, I_PUSH, "ptem") == 0);
  ASSERT (ioctl (slave, I_PUSH, "ldterm")  == 0);
# if defined __sun
  ASSERT (ioctl (slave, I_PUSH, "ttcompat") == 0);
# endif
#endif

  ASSERT (isatty (slave));

  /* Close the master side before the slave side gets closed.
     This is necessary on Mac OS X 10.4.11.  */
  ASSERT (close (master) == 0);
  ASSERT (close (slave) == 0);

  return test_exit_status;
}
