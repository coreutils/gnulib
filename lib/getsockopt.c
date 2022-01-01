/* getsockopt.c --- wrappers for Windows getsockopt function

   Copyright (C) 2008-2022 Free Software Foundation, Inc.

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

/* Written by Paolo Bonzini */

#include <config.h>

#define WIN32_LEAN_AND_MEAN
/* Get winsock2.h. */
#include <sys/socket.h>

/* Get struct timeval. */
#include <sys/time.h>

/* Get memcpy. */
#include <string.h>

/* Get set_winsock_errno, FD_TO_SOCKET etc. */
#include "w32sock.h"

#undef getsockopt

int
rpl_getsockopt (int fd, int level, int optname, void *optval, socklen_t *optlen)
{
  SOCKET sock = FD_TO_SOCKET (fd);

  if (sock == INVALID_SOCKET)
    {
      errno = EBADF;
      return -1;
    }
  else
    {
      int r;

      if (level == SOL_SOCKET
          && (optname == SO_RCVTIMEO || optname == SO_SNDTIMEO))
        {
          int milliseconds;
          int milliseconds_len = sizeof (int);
          struct timeval tv;
          size_t n;

          r = getsockopt (sock, level, optname, (char *) &milliseconds,
                          &milliseconds_len);
          tv.tv_sec = milliseconds / 1000;
          tv.tv_usec = (milliseconds - 1000 * tv.tv_sec) * 1000;
          n = sizeof (struct timeval);
          if (n > *optlen)
            n = *optlen;
          memcpy (optval, &tv, n);
          *optlen = n;
        }
      else
        {
          r = getsockopt (sock, level, optname, optval, optlen);
        }

      if (r < 0)
        set_winsock_errno ();

      return r;
    }
}
