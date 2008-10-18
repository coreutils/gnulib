/* Provide a netdb.h header file for systems lacking it (read: MinGW).
   Copyright (C) 2008 Free Software Foundation, Inc.
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

/* This file is supposed to be used on platforms that lack <netdb.h>.
   It is intended to provide definitions and prototypes needed by an
   application.  */

#ifndef _GL_NETDB_H

#if @HAVE_NETDB_H@

# if __GNUC__ >= 3
@PRAGMA_SYSTEM_HEADER@
# endif

/* The include_next requires a split double-inclusion guard.  */
# @INCLUDE_NEXT@ @NEXT_NETDB_H@

#endif

#ifndef _GL_NETDB_H
#define _GL_NETDB_H

#if @HAVE_NETDB_H@

/* Declarations for a platform that has <netdb.h>.  */

#else

/* Get netdb.h definitions such as struct hostent for MinGW.  */
#include <sys/socket.h>

/* Declarations for a platform that lacks <netdb.h>.  */

#endif /* HAVE_NETDB_H */

#endif /* _GL_NETDB_H */
#endif /* _GL_NETDB_H */
