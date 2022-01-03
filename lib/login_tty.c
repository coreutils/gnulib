/* Assign a given terminal as controlling terminal and as standard input,
   standard output, standard error of the current process.
   Copyright (C) 2010-2022 Free Software Foundation, Inc.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#include <config.h>

/* Currently no specification header.  */

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

int
login_tty (int slave_fd)
{
  int i;

  /* Create a new session.  */
  setsid ();

  /* Make fd the controlling terminal for the current process.
     On BSD and OSF/1: There is ioctl TIOCSCTTY for this purpose.
     On Solaris:
       A terminal becomes the controlling terminal of a session
       if it is being open()ed, at a moment when
         1. it is not already the controlling terminal of some session, and
         2. the process that open()s it is a session leader that does not have
            a controlling terminal.
       We assume condition 1, try to ensure condition 2, and then open() it.
   */
  for (i = 0; i < 3; i++)
    if (i != slave_fd)
      close (i);
#ifdef TIOCSCTTY
  if (ioctl (slave_fd, TIOCSCTTY, NULL) < 0)
    return -1;
#else
  {
    char *slave_name;
    int dummy_fd;

    slave_name = ttyname (slave_fd);
    if (slave_name == NULL)
      return -1;
    dummy_fd = open (slave_name, O_RDWR | O_CLOEXEC);
    if (dummy_fd < 0)
      return -1;
    close (dummy_fd);
  }
#endif

  /* Assign fd to the standard input, standard output, and standard error of
     the current process.  */
  for (i = 0; i < 3; i++)
    if (slave_fd != i)
      if (dup2 (slave_fd, i) < 0)
        return -1;
  if (slave_fd >= 3)
    close (slave_fd);

  return 0;
}
