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
#include "freadahead.h"

size_t
freadahead (FILE *fp)
{
#if defined _IO_ferror_unlocked     /* GNU libc, BeOS */
  if (fp->_IO_write_ptr > fp->_IO_write_base)
    return 0;
  return fp->_IO_read_end - fp->_IO_read_ptr;
#elif defined __sferror             /* FreeBSD, NetBSD, OpenBSD, MacOS X, Cygwin */
  if ((fp->_flags & __SWR) != 0 || fp->_r < 0)
    return 0;
  return fp->_r;
#elif defined _IOERR                /* AIX, HP-UX, IRIX, OSF/1, Solaris, mingw */
# if defined __sun && defined _LP64 /* Solaris/{SPARC,AMD64} 64-bit */
#  define fp_ ((struct { unsigned char *_ptr; \
			 unsigned char *_base; \
			 unsigned char *_end; \
			 long _cnt; \
			 int _file; \
			 unsigned int _flag; \
		       } *) fp)
  if ((fp_->_flag & _IOWRT) != 0)
    return 0;
  return fp_->_cnt;
# else
  if ((fp->_flag & _IOWRT) != 0)
    return 0;
  return fp->_cnt;
# endif
#elif defined __UCLIBC__            /* uClibc */
# ifdef __STDIO_BUFFERS
  if (fp->__modeflags & __FLAG_WRITING)
    return 0;
  return fp->__bufread - fp->__bufpos;
# else
  return 0;
# endif
#else
 #error "Please port gnulib freadahead.c to your platform! Look at the definition of fflush, fread on your system, then report this to bug-gnulib."
#endif
}
