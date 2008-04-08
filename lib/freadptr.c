/* Retrieve information about a FILE stream.
   Copyright (C) 2007-2008 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include <config.h>

/* Specification.  */
#include "freadptr.h"

const char *
freadptr (FILE *fp, size_t *sizep)
{
  size_t size;

  /* Keep this code in sync with freadahead!  */
#if defined _IO_ferror_unlocked     /* GNU libc, BeOS */
  if (fp->_IO_write_ptr > fp->_IO_write_base)
    return NULL;
  size = fp->_IO_read_end - fp->_IO_read_ptr;
  if (size == 0)
    return NULL;
  *sizep = size;
  return (const char *) fp->_IO_read_ptr;
#elif defined __sferror             /* FreeBSD, NetBSD, OpenBSD, MacOS X, Cygwin */
  if ((fp->_flags & __SWR) != 0 || fp->_r < 0)
    return NULL;
  size = fp->_r;
  if (size == 0)
    return NULL;
  *sizep = size;
  return (const char *) fp->_p;
#elif defined _IOERR                /* AIX, HP-UX, IRIX, OSF/1, Solaris, OpenServer, mingw */
# if defined __sun && defined _LP64 /* Solaris/{SPARC,AMD64} 64-bit */
#  define fp_ ((struct { unsigned char *_ptr; \
			 unsigned char *_base; \
			 unsigned char *_end; \
			 long _cnt; \
			 int _file; \
			 unsigned int _flag; \
		       } *) fp)
  if ((fp_->_flag & _IOWRT) != 0)
    return NULL;
  size = fp_->_cnt;
  if (size == 0)
    return NULL;
  *sizep = size;
  return (const char *) fp_->_ptr;
# else
#  if defined _SCO_DS               /* OpenServer */
#   define _flag __flag
#   define _ptr __ptr
#   define _cnt __cnt
#  endif
  if ((fp->_flag & _IOWRT) != 0)
    return NULL;
  size = fp->_cnt;
  if (size == 0)
    return NULL;
  *sizep = size;
  return (const char *) fp->_ptr;
# endif
#elif defined __UCLIBC__            /* uClibc */
# ifdef __STDIO_BUFFERS
  if (fp->__modeflags & __FLAG_WRITING)
    return NULL;
  size = fp->__bufread - fp->__bufpos;
  if (size == 0)
    return NULL;
  *sizep = size;
  return (const char *) fp->__bufpos;
# else
  return NULL;
# endif
#elif defined __QNX__               /* QNX */
  if ((fp->_Mode & 0x2000 /* _MWRITE */) != 0)
    return NULL;
  /* fp->_Buf <= fp->_Next <= fp->_Rend */
  size = fp->_Rend - fp->_Next;
  if (size == 0)
    return NULL;
  *sizep = size;
  return (const char *) fp->_Next;
#else
 #error "Please port gnulib freadptr.c to your platform! Look at the definition of fflush, fread, getc, getc_unlocked on your system, then report this to bug-gnulib."
#endif
}
