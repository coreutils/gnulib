/* Open the master side of a pseudo-terminal.
   Copyright (C) 2010-2022 Free Software Foundation, Inc.

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

/* Specification.  */
#include <stdlib.h>

#include <errno.h>
#include <fcntl.h>
#if defined __OpenBSD__
# include <sys/ioctl.h>
# include <sys/tty.h>
#endif

/* posix_openpt() is already provided on
     glibc >= 2.2.1 (but is a stub on GNU/Hurd),
     Mac OS X >= 10.4,
     FreeBSD >= 5.1 (lived in src/lib/libc/stdlib/grantpt.c before 8.0),
     NetBSD >= 3.0,
     AIX >= 5.2, HP-UX >= 11.31, Solaris >= 10, Cygwin >= 1.7.
   Thus, this replacement function is compiled on
     Mac OS X 10.3, OpenBSD 4.9, Minix 3.1.8,
     AIX 5.1, HP-UX 11.23, IRIX 6.5, OSF/1 5.1, Solaris 9,
     Cygwin 1.5.x, mingw, MSVC 9, Interix 3.5, BeOS.
   Among these:
     - AIX has /dev/ptc.
     - HP-UX 10..11, IRIX 6.5, OSF/1 5.1, Solaris 2.6..9, Cygwin 1.5
       have /dev/ptmx.
     - HP-UX 10..11 also has /dev/ptym/clone, but this should not be needed.
     - OpenBSD 4.9 has /dev/ptm and the PTMGET ioctl.
     - Minix 3.1.8 have a list of pseudo-terminal devices in /dev.
     - On native Windows, there are no ttys at all.  */

int
posix_openpt (int flags)
{
  int master;

#ifdef _AIX /* AIX */

  master = open ("/dev/ptc", flags);

#elif defined _WIN32 && !defined __CYGWIN__ /* mingw */

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

#else /* Mac OS X, Minix, HP-UX, IRIX, OSF/1, Solaris 9, Cygwin 1.5 */

  /* Most systems that lack posix_openpt() have /dev/ptmx.  */
  master = open ("/dev/ptmx", flags);

  /* If all this does not work, we could try to open, one by one:
     - On Mac OS X: /dev/pty[p-w][0-9a-f]
     - On *BSD:     /dev/pty[p-sP-S][0-9a-v]
     - On Minix:    /dev/pty[p-q][0-9a-f]
     - On AIX:      /dev/ptyp[0-9a-f]
     - On HP-UX:    /dev/pty[p-r][0-9a-f]
     - On OSF/1:    /dev/pty[p-q][0-9a-f]
     - On Solaris:  /dev/pty[p-r][0-9a-f]
   */

#endif

  return master;
}
