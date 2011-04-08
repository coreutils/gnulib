/* ioctl.c --- wrappers for Windows ioctl function

   Copyright (C) 2008-2011 Free Software Foundation, Inc.

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

/* Written by Paolo Bonzini */

#include <config.h>

#include <sys/ioctl.h>

#include <stdarg.h>

#if HAVE_IOCTL

/* Provide a wrapper with the POSIX prototype.  */
# undef ioctl
int
rpl_ioctl (int fd, int request, ... /* {void *,char *} arg */)
{
  void *buf;
  va_list args;

  va_start (args, request);
  buf = va_arg (args, void *);
  va_end (args);

  /* Cast 'request' so that when the system's ioctl function takes a 64-bit
     request argument, the value gets zero-extended, not sign-extended.  */
  return ioctl (fd, (unsigned int) request, buf);
}

#else /* mingw */

# define WIN32_LEAN_AND_MEAN
/* Get winsock2.h. */
# include <sys/socket.h>

/* Get set_winsock_errno, FD_TO_SOCKET etc. */
# include "w32sock.h"

int
ioctl (int fd, int req, ...)
{
# if GNULIB_SOCKET
  void *buf;
  va_list args;
  SOCKET sock;
  int r;

  va_start (args, req);
  buf = va_arg (args, void *);
  va_end (args);

  /* We don't support FIONBIO on pipes here.  If you want to make pipe
     fds non-blocking, use the gnulib 'nonblocking' module, until
     gnulib implements fcntl F_GETFL / F_SETFL with O_NONBLOCK.  */

  sock = FD_TO_SOCKET (fd);
  r = ioctlsocket (sock, req, buf);
  if (r < 0)
    set_winsock_errno ();

  return r;

# else
  errno = ENOSYS;
  return -1;
# endif
}

#endif
