/* Header for poll(2) emulation
   Contributed by Paolo Bonzini.

   Copyright 2001-2003, 2007, 2009-2022 Free Software Foundation, Inc.

   This file is part of gnulib.

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

#ifndef _@GUARD_PREFIX@_POLL_H

#if __GNUC__ >= 3
@PRAGMA_SYSTEM_HEADER@
#endif
@PRAGMA_COLUMNS@

/* The include_next requires a split double-inclusion guard.  */
#if @HAVE_POLL_H@
# @INCLUDE_NEXT@ @NEXT_POLL_H@
#endif

#ifndef _@GUARD_PREFIX@_POLL_H
#define _@GUARD_PREFIX@_POLL_H

/* On native Windows, get the 'struct pollfd' type and the POLL* macro
   definitions before we override them.  mingw defines them in <winsock2.h>
   if _WIN32_WINNT >= 0x0600.  */
#if @HAVE_WINSOCK2_H@
# include <winsock2.h>
#endif


/* The definitions of _GL_FUNCDECL_RPL etc. are copied here.  */

/* The definition of _GL_WARN_ON_USE is copied here.  */


#if !@HAVE_POLL_H@

# if @HAVE_WINSOCK2_H@
/* Override the definitions from <winsock2.h>.  */
#  undef POLLIN
#  undef POLLPRI
#  undef POLLOUT
#  undef POLLERR
#  undef POLLHUP
#  undef POLLNVAL
#  undef POLLRDNORM
#  undef POLLRDBAND
#  undef POLLWRNORM
#  undef POLLWRBAND
#  define pollfd rpl_pollfd
# endif

/* fake a poll(2) environment */
# define POLLIN      0x0001      /* any readable data available   */
# define POLLPRI     0x0002      /* OOB/Urgent readable data      */
# define POLLOUT     0x0004      /* file descriptor is writable   */
# define POLLERR     0x0008      /* some poll error occurred      */
# define POLLHUP     0x0010      /* file descriptor was "hung up" */
# define POLLNVAL    0x0020      /* requested events "invalid"    */
# define POLLRDNORM  0x0040
# define POLLRDBAND  0x0080
# define POLLWRNORM  0x0100
# define POLLWRBAND  0x0200

# if !GNULIB_defined_poll_types

struct pollfd
{
  int fd;                       /* which file descriptor to poll */
  short events;                 /* events we are interested in   */
  short revents;                /* events found on return        */
};

typedef unsigned long nfds_t;

#  define GNULIB_defined_poll_types 1
# endif

/* Define INFTIM only if doing so conforms to POSIX.  */
# if !defined (_POSIX_C_SOURCE) && !defined (_XOPEN_SOURCE)
#  define INFTIM (-1)
# endif

#endif


#if @GNULIB_POLL@
# if @REPLACE_POLL@
#  if !(defined __cplusplus && defined GNULIB_NAMESPACE)
#   undef poll
#   define poll rpl_poll
#  endif
_GL_FUNCDECL_RPL (poll, int, (struct pollfd *pfd, nfds_t nfd, int timeout));
_GL_CXXALIAS_RPL (poll, int, (struct pollfd *pfd, nfds_t nfd, int timeout));
# else
#  if !@HAVE_POLL@
_GL_FUNCDECL_SYS (poll, int, (struct pollfd *pfd, nfds_t nfd, int timeout));
#  endif
_GL_CXXALIAS_SYS (poll, int, (struct pollfd *pfd, nfds_t nfd, int timeout));
# endif
_GL_CXXALIASWARN (poll);
#elif defined GNULIB_POSIXCHECK
# undef poll
# if HAVE_RAW_DECL_POLL
_GL_WARN_ON_USE (poll, "poll is unportable - "
                 "use gnulib module poll for portability");
# endif
#endif


#endif /* _@GUARD_PREFIX@_POLL_H */
#endif /* _@GUARD_PREFIX@_POLL_H */
