/* Substitute for <sys/uio.h>.
   Copyright (C) 2011 Free Software Foundation, Inc.

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

# if __GNUC__ >= 3
@PRAGMA_SYSTEM_HEADER@
# endif
@PRAGMA_COLUMNS@

#ifndef _GL_SYS_UIO_H

#if @HAVE_SYS_UIO_H@

/* The include_next requires a split double-inclusion guard.  */
# @INCLUDE_NEXT@ @NEXT_SYS_UIO_H@

#endif

#ifndef _GL_SYS_UIO_H
#define _GL_SYS_UIO_H

#if !@HAVE_SYS_UIO_H@
/* A platform that lacks <sys/uio.h>.  */
/* Get 'ssize_t'.  */
# include <sys/types.h>

/* All known platforms that lack <sys/uio.h> also lack any declaration
   of struct iovec in any other header.  */
struct iovec {
  void *iov_base;
  size_t iov_len;
};
#endif

#endif /* _GL_SYS_UIO_H */
#endif /* _GL_SYS_UIO_H */
