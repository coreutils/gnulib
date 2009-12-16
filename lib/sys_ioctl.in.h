/* Substitute for and wrapper around <sys/ioctl.h>.
   Copyright (C) 2008-2010 Free Software Foundation, Inc.

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

#ifndef _GL_SYS_IOCTL_H

#if __GNUC__ >= 3
@PRAGMA_SYSTEM_HEADER@
#endif

/* The include_next requires a split double-inclusion guard.  */
#if @HAVE_SYS_IOCTL_H@
# @INCLUDE_NEXT@ @NEXT_SYS_IOCTL_H@
#endif

#ifndef _GL_SYS_IOCTL_H
#define _GL_SYS_IOCTL_H

/* AIX 5.1 and Solaris 10 declare ioctl() in <unistd.h> and in <stropts.h>,
   but not in <sys/ioctl.h>.
   But avoid namespace pollution on glibc systems.  */
#ifndef __GLIBC__
# include <unistd.h>
#endif

/* The definition of _GL_WARN_ON_USE is copied here.  */


/* Declare overridden functions.  */

#ifdef __cplusplus
extern "C" {
#endif


#if @GNULIB_IOCTL@
# if @SYS_IOCTL_H_HAVE_WINSOCK2_H@
#  undef ioctl
#  define ioctl rpl_ioctl
extern int ioctl (int fd, int request, ... /* {void *,char *} arg */);
# endif
#elif @SYS_IOCTL_H_HAVE_WINSOCK2_H_AND_USE_SOCKETS@
# undef ioctl
# define ioctl ioctl_used_without_requesting_gnulib_module_ioctl
#elif defined GNULIB_POSIXCHECK
# undef ioctl
# if HAVE_RAW_DECL_IOCTL
_GL_WARN_ON_USE (ioctl, "ioctl does not portably work on sockets - "
                 "use gnulib module ioctl for portability");
# endif
#endif


#ifdef __cplusplus
}
#endif


#endif /* _GL_SYS_IOCTL_H */
#endif /* _GL_SYS_IOCTL_H */
