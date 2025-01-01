/* A GNU-like <utmp.h>.

   Copyright (C) 2023-2025 Free Software Foundation, Inc.

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

#if __GNUC__ >= 3
@PRAGMA_SYSTEM_HEADER@
#endif
@PRAGMA_COLUMNS@

#ifndef _@GUARD_PREFIX@_UTMP_H

#include <sys/types.h>

/* The include_next requires a split double-inclusion guard.  */
#if @HAVE_UTMP_H@
# @INCLUDE_NEXT@ @NEXT_UTMP_H@
#endif

#ifndef _@GUARD_PREFIX@_UTMP_H
#define _@GUARD_PREFIX@_UTMP_H

/* This file uses GNULIB_POSIXCHECK, HAVE_RAW_DECL_*.  */
#if !_GL_CONFIG_H_INCLUDED
 #error "Please include config.h first."
#endif

/* FreeBSD, NetBSD, OpenBSD, macOS, Minix, Haiku declare login_tty in
   <util.h> or <libutil.h>, not in <utmp.h>.  Solaris declares it in
   <termios.h>, not in <utmp.h>.  */
/* But in any case avoid namespace pollution on glibc systems.  */
#if (@GNULIB_LOGIN_TTY@ || defined GNULIB_POSIXCHECK) && ! defined __GLIBC__
# if @HAVE_UTIL_H@      /* macOS, NetBSD, OpenBSD, Minix */
#  include <util.h>
# elif @HAVE_LIBUTIL_H@ /* FreeBSD, Haiku */
#  include <libutil.h>
# elif @HAVE_TERMIOS_H@ /* Solaris */
#  include <termios.h>
# endif
#endif

/* The definitions of _GL_FUNCDECL_RPL etc. are copied here.  */

/* The definition of _GL_WARN_ON_USE is copied here.  */


#if @GNULIB_LOGIN_TTY@
# if @REPLACE_LOGIN_TTY@
#  if !(defined __cplusplus && defined GNULIB_NAMESPACE)
#   undef login_tty
#   define login_tty rpl_login_tty
#  endif
_GL_FUNCDECL_RPL (login_tty, int, (int fd), );
_GL_CXXALIAS_RPL (login_tty, int, (int fd));
# else
#  if !@HAVE_LOGIN_TTY@
_GL_FUNCDECL_SYS (login_tty, int, (int fd), );
#  endif
_GL_CXXALIAS_SYS (login_tty, int, (int fd));
# endif
# if __GLIBC__ >= 2
_GL_CXXALIASWARN (login_tty);
# endif
#elif defined GNULIB_POSIXCHECK
# undef login_tty
# if HAVE_RAW_DECL_LOGIN_TTY
_GL_WARN_ON_USE (login_tty, "login_tty is unportable - "
                 "use gnulib module login_tty for portability");
# endif
#endif


#endif /* _@GUARD_PREFIX@_UTMP_H */
#endif /* _@GUARD_PREFIX@_UTMP_H */
