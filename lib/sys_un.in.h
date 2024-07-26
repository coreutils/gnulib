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

#if __GNUC__ >= 3
@PRAGMA_SYSTEM_HEADER@
#endif
@PRAGMA_COLUMNS@

#if @HAVE_SYS_UN_H@

/* On glibc, in C++ mode with clang, <stddef.h> and <string.h> need to be
   included before <sys/un.h>, because otherwise <sys/un.h> includes them inside
   an 'extern "C" { ... }' block, which leads to compilation errors in gnulib's
   <string.h> override.  */
# if defined __cplusplus
#  include <stddef.h>
#  include <string.h>
# endif

/* The include_next requires a split double-inclusion guard.  */
# @INCLUDE_NEXT@ @NEXT_SYS_UN_H@

#endif

#ifndef _@GUARD_PREFIX@_SYS_UN_H
#define _@GUARD_PREFIX@_SYS_UN_H 1

#if !@HAVE_SYS_UN_H@

/* A platform that lacks <sys/un.h>.  */

/* Windows requires <winsock2.h> to be included before <afunix.h>.  */
# if @HAVE_WINSOCK2_H@
#  include <winsock2.h>
# endif
# if @HAVE_AFUNIX_H@
#  include <afunix.h>
# endif

/* If a platform does not support AF_UNIX sockets 'struct sockaddr_un' will
   not be defined.  You may use HAVE_UNIXSOCKET after including <config.h>.  */

#endif

#endif /* _@GUARD_PREFIX@_SYS_UN_H */
#endif /* _@GUARD_PREFIX@_SYS_UN_H */
