/* Determine the session ID of the controlling terminal of the current process.
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

#define USE_OLD_TTY /* needed on OpenBSD 4.5, so that TIOCGSID gets defined */

/* Specification.  */
#include <termios.h>

#include <errno.h>
#include <sys/ioctl.h>

pid_t
tcgetsid (int fd)
{
#ifdef TIOCGSID /* Mac OS X, OpenBSD */
  int sid;

  if (ioctl (fd, TIOCGSID, &sid) < 0)
    return -1; /* errno is set here */

  return sid;
#else /* FreeBSD, Cygwin, mingw */
  errno = ENOSYS;
  return -1;
#endif
}
