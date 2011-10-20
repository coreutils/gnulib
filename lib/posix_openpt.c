/* Open the master side of a pseudo-terminal.
   Copyright (C) 2010-2011 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include <config.h>

/* Specification.  */
#include <stdlib.h>

#include <errno.h>
#include <fcntl.h>
#if defined __OpenBSD__
# include <sys/ioctl.h>
# include <sys/tty.h>
#endif

int
posix_openpt (int flags)
{
  int master;

#ifdef _AIX /* AIX */

  master = open ("/dev/ptc", flags);

#elif (defined _WIN32 || defined __WIN32__) && !defined __CYGWIN__ /* mingw */

  /* Mingw lacks pseudo-terminals altogether.  */
  master = -1;
  errno = ENOSYS;

#elif defined __OpenBSD__

  /* On OpenBSD, master and slave of a pseudo-terminal are allocated together,
     by opening /dev/ptm and applying the PTMGET ioctl to it.  */
  int fd;
  struct ptmget data;

  fd = open (PATH_PTMDEV, O_RDWR);
  if (fd >= 0)
    {
      if (ioctl (fd, PTMGET, &data) >= 0)
        {
          master = data.cfd;
          close (data.sfd);
          close (fd);
        }
      else
        {
          int saved_errno = errno;
          close (fd);
          errno = saved_errno;
          master = -1;
        }
    }
  else
    master = -1;

#else /* MacOS X, Minix, HP-UX, IRIX, OSF/1, Solaris 9, Cygwin 1.5 */

  /* Most systems that lack posix_openpt() have /dev/ptmx.  */
  master = open ("/dev/ptmx", flags);

#endif

  return master;
}
