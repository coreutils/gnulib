/* Flushing buffers of a FILE stream.
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
#include "fpurge.h"

int
fpurge (FILE *fp)
{
  /* Most systems provide FILE as a struct and the necessary bitmask in
     <stdio.h>, because they need it for implementing getc() and putc() as
     fast macros.  */
#if defined _IO_ferror_unlocked     /* GNU libc, BeOS */
  fp->_IO_read_end = fp->_IO_read_ptr;
  fp->_IO_write_ptr = fp->_IO_write_base;
  return 0;
#elif defined __sferror             /* FreeBSD, NetBSD, OpenBSD, MacOS X, Cygwin */
  fp->_p = fp->_bf._base;
  fp->_r = 0;
  fp->_w = ((fp->_flags & (__SLBF | __SNBF)) == 0 /* fully buffered? */
	    ? fp->_bf._size
	    : 0);
  return 0;
#elif defined _IOERR                /* AIX, HP-UX, IRIX, OSF/1, Solaris, mingw */
  fp->_ptr = fp->_base;
  if (fp->_ptr != NULL)
    fp->_cnt = 0;
  return 0;
#else
 #error "Please port gnulib fpurge.c to your platform!"
#endif
}
