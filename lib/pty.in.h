/* Substitute for and wrapper around <pty.h>.
   Copyright (C) 2010 Free Software Foundation, Inc.

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

#ifndef _GL_PTY_H

#if __GNUC__ >= 3
@PRAGMA_SYSTEM_HEADER@
#endif

/* The include_next requires a split double-inclusion guard.  */
#if @HAVE_PTY_H@
# @INCLUDE_NEXT@ @NEXT_PTY_H@
#endif

#ifndef _GL_PTY_H
#define _GL_PTY_H

/* Some platforms declare this in a different header than glibc.  */
#if @HAVE_UTIL_H@
# include <util.h>
#endif
#if @HAVE_LIBUTIL_H@
# include <libutil.h>
#endif

/* The definitions of _GL_FUNCDECL_RPL etc. are copied here.  */

/* The definition of _GL_WARN_ON_USE is copied here.  */


/* Declare overridden functions.  */

#if @GNULIB_FORKPTY@
#elif defined GNULIB_POSIXCHECK
# undef forkpty
# if HAVE_RAW_DECL_FORKPTY
_GL_WARN_ON_USE (forkpty, "forkpty is not declared consistently - "
                 "use gnulib module forkpty for portability");
# endif
#endif

#if @GNULIB_OPENPTY@
#elif defined GNULIB_POSIXCHECK
# undef openpty
# if HAVE_RAW_DECL_OPENPTY
_GL_WARN_ON_USE (openpty, "openpty is not declared consistently - "
                 "use gnulib module openpty for portability");
# endif
#endif


#endif /* _GL_PTY_H */
#endif /* _GL_PTY_H */
