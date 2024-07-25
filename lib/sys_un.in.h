/* sys/un.h - definitions for UNIX domain sockets

   Copyright 2024 Free Software Foundation, Inc.

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

/* Written by Collin Funk.  */

#ifndef _@GUARD_PREFIX@_SYS_UN_H
#define _@GUARD_PREFIX@_SYS_UN_H 1

/* Windows requires <winsock2.h> to be included before <afunix.h>.  */
#if @HAVE_WINSOCK2_H@
# include <winsock2.h>
#endif
#if @HAVE_AFUNIX_H@
# include <afunix.h>
#endif

/* If a platform does not support AF_UNIX sockets 'struct sockaddr_un' will
   not be defined.  You may use HAVE_UNIXSOCKET after including <config.h>.  */

#endif
