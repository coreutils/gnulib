/* Provide a sys/socket header file for systems lacking it (read: MinGW)
   and for systems where it is incomplete.
   Copyright (C) 2005-2008 Free Software Foundation, Inc.
   Written by Simon Josefsson.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

/* This file is supposed to be used on platforms that lack <sys/socket.h>,
   on platforms where <sys/socket.h> cannot be included standalone, and on
   platforms where <sys/socket.h> does not provide all necessary definitions.
   It is intended to provide definitions and prototypes needed by an
   application.  */

#ifndef _GL_SYS_SOCKET_H

#if @HAVE_SYS_SOCKET_H@

@PRAGMA_SYSTEM_HEADER@

/* On many platforms, <sys/socket.h> assumes prior inclusion of
   <sys/types.h>.  */
# include <sys/types.h>

/* The include_next requires a split double-inclusion guard.  */
# @INCLUDE_NEXT@ @NEXT_SYS_SOCKET_H@

#endif

#ifndef _GL_SYS_SOCKET_H
#define _GL_SYS_SOCKET_H

#if @HAVE_SYS_SOCKET_H@

/* A platform that has <sys/socket.h>.  */

/* For shutdown().  */
# if !defined SHUT_RD
#  define SHUT_RD 0
# endif
# if !defined SHUT_WR
#  define SHUT_WR 1
# endif
# if !defined SHUT_RDWR
#  define SHUT_RDWR 2
# endif

#else

# ifdef __CYGWIN__
#  error "Cygwin does have a sys/socket.h, doesn't it?!?"
# endif

/* A platform that lacks <sys/socket.h>.

   Currently only MinGW is supported.  See the gnulib manual regarding
   Windows sockets.  MinGW has the header files winsock2.h and
   ws2tcpip.h that declare the sys/socket.h definitions we need.  Note
   that you can influence which definitions you get by setting the
   WINVER symbol before including these two files.  For example,
   getaddrinfo is only available if _WIN32_WINNT >= 0x0501 (that
   symbol is set indiriectly through WINVER).  You can set this by
   adding AC_DEFINE(WINVER, 0x0501) to configure.ac.  Note that your
   code may not run on older Windows releases then.  My Windows 2000
   box was not able to run the code, for example.  The situation is
   slightly confusing because:
   http://msdn.microsoft.com/library/default.asp?url=/library/en-us/winsock/winsock/getaddrinfo_2.asp
   suggests that getaddrinfo should be available on all Windows
   releases. */


# if @HAVE_WINSOCK2_H@
#  include <winsock2.h>
# endif
# if @HAVE_WS2TCPIP_H@
#  include <ws2tcpip.h>
# endif

/* For shutdown(). */
# if !defined SHUT_RD && defined SD_RECEIVE
#  define SHUT_RD SD_RECEIVE
# endif
# if !defined SHUT_WR && defined SD_SEND
#  define SHUT_WR SD_SEND
# endif
# if !defined SHUT_RDWR && defined SD_BOTH
#  define SHUT_RDWR SD_BOTH
# endif

# if @HAVE_WINSOCK2_H@
/* Include headers needed by the emulation code.  */
#  include <sys/types.h>
#  include <io.h>

typedef int socklen_t;

#  ifdef __cplusplus
extern "C" {
#  endif

/* Re-define FD_ISSET to avoid a WSA call while we are not using
   network sockets.  */
static inline int
rpl_fd_isset (int fd, fd_set * set)
{
  int i;
  if (set == NULL)
    return 0;

  for (i = 0; i < set->fd_count; i++)
    if (set->fd_array[i] == fd)
      return 1;

  return 0;
}

#  undef FD_ISSET
#  define FD_ISSET(fd, set) rpl_fd_isset(fd, set)

/* Wrap everything else to use libc file descriptors for sockets.  */

#  undef close
#  define close			rpl_close
#  undef socket
#  define socket		rpl_socket
#  undef connect
#  define connect		rpl_connect
#  undef accept
#  define accept		rpl_accept
#  undef bind
#  define bind			rpl_bind
#  undef getpeername
#  define getpeername		rpl_getpeername
#  undef getsockname
#  define getsockname		rpl_getsockname
#  undef getsockopt
#  define getsockopt		rpl_getsockopt
#  undef listen
#  define listen		rpl_listen
#  undef ioctl
#  define ioctl			rpl_ioctl
#  undef recv
#  define recv			rpl_recv
#  undef send
#  define send			rpl_send
#  undef recvfrom
#  define recvfrom		rpl_recvfrom
#  undef sendto
#  define sendto		rpl_sendto
#  undef setsockopt
#  define setsockopt		rpl_setsockopt
#  undef select
#  define select		select_used_without_including_sys_select_h

extern int rpl_close(int);
extern int rpl_socket (int, int, int protocol);
extern int rpl_connect (int, struct sockaddr *, int);
extern int rpl_accept (int, struct sockaddr *, int *);
extern int rpl_bind (int, struct sockaddr *, int);
extern int rpl_getpeername (int, struct sockaddr *, int *);
extern int rpl_getsockname (int, struct sockaddr *, int *);
extern int rpl_getsockopt (int, int, int, void *, int *);
extern int rpl_listen (int, int);
extern int rpl_ioctl (int, unsigned long, char *);
extern int rpl_recv (int, void *, int, int);
extern int rpl_send (int, const void *, int, int);
extern int rpl_recvfrom (int, void *, int, int, struct sockaddr *, int *);
extern int rpl_sendto (int, const void *, int, int, struct sockaddr *, int);
extern int rpl_setsockopt (int, int, int, const void *, int);

#  ifdef __cplusplus
}
#  endif

# endif /* HAVE_WINSOCK2_H */

#endif /* HAVE_SYS_SOCKET_H */

#endif /* _GL_SYS_SOCKET_H */
#endif /* _GL_SYS_SOCKET_H */
