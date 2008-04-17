/* Set the error indicator of a stream.
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
#include "fseterr.h"

#include <errno.h>

void
fseterr (FILE *fp)
{
  /* Most systems provide FILE as a struct and the necessary bitmask in
     <stdio.h>, because they need it for implementing getc() and putc() as
     fast macros.  */
#if defined _IO_ferror_unlocked || __GNU_LIBRARY__ == 1 /* GNU libc, BeOS, Linux libc5 */
  fp->_flags |= _IO_ERR_SEEN;
#elif defined __sferror             /* FreeBSD, NetBSD, OpenBSD, MacOS X, Cygwin */
  fp->_flags |= __SERR;
#elif defined __EMX__               /* emx+gcc */
  fp->_flags |= _IOERR;
#elif defined _IOERR                /* AIX, HP-UX, IRIX, OSF/1, Solaris, OpenServer, mingw */
# if defined __sun && defined _LP64 /* Solaris/{SPARC,AMD64} 64-bit */
#  define fp_ ((struct { unsigned char *_ptr; \
			 unsigned char *_base; \
			 unsigned char *_end; \
			 long _cnt; \
			 int _file; \
			 unsigned int _flag; \
		       } *) fp)
  fp_->_flag |= _IOERR;
# else
#  if defined _SCO_DS               /* OpenServer */
#   define _flag __flag
#  endif
  fp->_flag |= _IOERR;
# endif
#elif defined __UCLIBC__            /* uClibc */
  fp->__modeflags |= __FLAG_ERROR;
#elif defined __QNX__               /* QNX */
  fp->_Mode |= 0x200 /* _MERR */;
#elif 0                             /* unknown  */
  /* Portable fallback, based on an idea by Rich Felker.
     Wow! 6 system calls for something that is just a bit operation!
     Not activated on any system, because there is no way to repair FP when
     the sequence of system calls fails, and library code should not call
     abort().  */
  int saved_errno;
  int fd;
  int fd2;

  saved_errno = errno;
  fflush (fp);
  fd = fileno (fp);
  fd2 = dup (fd);
  if (fd2 >= 0)
    {
      close (fd);
      fputc ('\0', fp); /* This should set the error indicator.  */
      fflush (fp);      /* Or this.  */
      if (dup2 (fd2, fd) < 0)
	/* Whee... we botched the stream and now cannot restore it!  */
	abort ();
      close (fd2);
    }
  errno = saved_errno;
#else
 #error "Please port gnulib fseterr.c to your platform! Look at the definitions of ferror and clearerr on your system, then report this to bug-gnulib."
#endif
}
