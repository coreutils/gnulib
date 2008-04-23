/* Provide a arpa/inet header file for systems lacking it (read: MinGW)
   Copyright (C) 2008 Free Software Foundation, Inc.

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

#ifndef _GL_ARPA_INET_H
#define _GL_ARPA_INET_H

/* Gnulib's sys/socket.h is responsible for pulling in winsock2.h etc
   under MinGW. */
#include <sys/socket.h>

#if @GNULIB_INET_NTOP@
# include <inet_ntop.h>
#elif defined GNULIB_POSIXCHECK
# undef inet_ntop
# define inet_ntop(af,src,dst,cnt)					\
    (GL_LINK_WARNING ("inet_ntop doesn't exist on mingw - " \
                      "use gnulib module inet_ntop for portability"), \
     inet_ntop (af, src, dst, cnt))
#endif

#if @GNULIB_INET_PTON@
# include <inet_pton.h>
#elif defined GNULIB_POSIXCHECK
# undef inet_pton
# define inet_pton(af,src,dst)			    \
  (GL_LINK_WARNING ("inet_pton doesn't exist on mingw - "	  \
		    "use gnulib module inet_pton for portability"),	  \
   inet_pton (af, src, dst))
#endif

#endif /* _GL_ARPA_INET_H */
