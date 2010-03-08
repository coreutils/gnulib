/* Test of <sys/socket.h> substitute in C++ mode.
   Copyright (C) 2010 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2010.  */

#define GNULIB_NAMESPACE gnulib
#include <config.h>

#include <sys/socket.h>

#include "signature.h"


#if GNULIB_SOCKET
SIGNATURE_CHECK (GNULIB_NAMESPACE::socket, int, (int, int, int));
#endif

#if GNULIB_CONNECT
SIGNATURE_CHECK (GNULIB_NAMESPACE::connect, int,
                 (int, const struct sockaddr *, socklen_t));
#endif

#if GNULIB_ACCEPT
SIGNATURE_CHECK (GNULIB_NAMESPACE::accept, int,
                 (int, struct sockaddr *, socklen_t *));
#endif

#if GNULIB_BIND
SIGNATURE_CHECK (GNULIB_NAMESPACE::bind, int,
                 (int, const struct sockaddr *, socklen_t));
#endif

#if GNULIB_GETPEERNAME
SIGNATURE_CHECK (GNULIB_NAMESPACE::getpeername, int,
                 (int, struct sockaddr *, socklen_t *));
#endif

#if GNULIB_GETSOCKNAME
SIGNATURE_CHECK (GNULIB_NAMESPACE::getsockname, int,
                 (int, struct sockaddr *, socklen_t *));
#endif

#if GNULIB_GETSOCKOPT
SIGNATURE_CHECK (GNULIB_NAMESPACE::getsockopt, int,
                 (int, int, int, void *, socklen_t *));
#endif

#if GNULIB_LISTEN
SIGNATURE_CHECK (GNULIB_NAMESPACE::listen, int, (int, int));
#endif

#if GNULIB_RECV
SIGNATURE_CHECK (GNULIB_NAMESPACE::recv, ssize_t, (int, void *, size_t, int));
#endif

#if GNULIB_SEND
SIGNATURE_CHECK (GNULIB_NAMESPACE::send, ssize_t,
                 (int, const void *, size_t, int));
#endif

#if GNULIB_RECVFROM
SIGNATURE_CHECK (GNULIB_NAMESPACE::recvfrom, ssize_t,
                 (int, void *, size_t, int, struct sockaddr *, socklen_t *));
#endif

#if GNULIB_SENDTO
SIGNATURE_CHECK (GNULIB_NAMESPACE::sendto, ssize_t,
                 (int, const void *, size_t, int,
                  const struct sockaddr *, socklen_t));
#endif

#if GNULIB_SETSOCKOPT
SIGNATURE_CHECK (GNULIB_NAMESPACE::setsockopt, int,
                 (int, int, int, const void *, socklen_t));
#endif

#if GNULIB_SHUTDOWN
SIGNATURE_CHECK (GNULIB_NAMESPACE::shutdown, int, (int, int));
#endif

#if GNULIB_ACCEPT4
SIGNATURE_CHECK (GNULIB_NAMESPACE::accept4, int,
                 (int, struct sockaddr *, socklen_t *, int));
#endif


int
main ()
{
}
