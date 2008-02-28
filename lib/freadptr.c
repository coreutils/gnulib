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
#include "freadahead.h"

const char *
freadptr (FILE *fp)
{
  /* Keep this code in sync with freadahead!  */
#if defined _IO_ferror_unlocked     /* GNU libc, BeOS */
  return (const char *) fp->_IO_read_ptr;
#elif defined __sferror             /* FreeBSD, NetBSD, OpenBSD, MacOS X, Cygwin */
  return (const char *) fp->_p;
#elif defined _IOERR                /* AIX, HP-UX, IRIX, OSF/1, Solaris, mingw */
# if defined __sun && defined _LP64 /* Solaris/{SPARC,AMD64} 64-bit */
#  define fp_ ((struct { unsigned char *_ptr; \
			 unsigned char *_base; \
			 unsigned char *_end; \
			 long _cnt; \
			 int _file; \
			 unsigned int _flag; \
		       } *) fp)
  return (const char *) fp_->_ptr;
# else
  return (const char *) fp->_ptr;
# endif
#elif defined __UCLIBC__            /* uClibc */
# ifdef __STDIO_BUFFERS
  return (const char *) fp->__bufpos;
# else
  return NULL;
# endif
#elif defined __QNX__               /* QNX */
  return (const char *) fp->_Next;
#else
 #error "Please port gnulib freadptr.c to your platform! Look at the definition of getc, getc_unlocked on your system, then report this to bug-gnulib."
#endif
}
