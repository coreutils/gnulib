/* Retrieve information about a FILE stream.
   Copyright (C) 2007 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

#include <config.h>

/* Specification.  */
#include "freading.h"

/* Don't use glibc's __freading function, see
   <http://sourceware.org/bugzilla/show_bug.cgi?id=4359>  */
#if !(HAVE___FREADING && !defined __GLIBC__)

bool
freading (FILE *fp)
{
  /* Most systems provide FILE as a struct and the necessary bitmask in
     <stdio.h>, because they need it for implementing getc() and putc() as
     fast macros.  */
#if defined _IO_ferror_unlocked     /* GNU libc, BeOS */
  return ((fp->_flags & _IO_NO_WRITES) != 0
	  || ((fp->_flags & (_IO_NO_READS | _IO_CURRENTLY_PUTTING)) == 0
	      && fp->_IO_read_base != NULL));
#elif defined __sferror             /* FreeBSD, NetBSD, OpenBSD, MacOS X, Cygwin */
  return (fp->_flags & __SRD) != 0;
#elif defined _IOERR                /* AIX, HP-UX, IRIX, OSF/1, Solaris, mingw */
  return (fp->_flag & _IOREAD) != 0;
#elif defined __UCLIBC__            /* uClibc */
  return (fp->__modeflags & (__FLAG_READONLY | __FLAG_READING)) != 0;
#else
 #error "Please port gnulib freading.c to your platform!"
#endif
}

#endif
