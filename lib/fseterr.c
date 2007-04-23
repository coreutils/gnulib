/* Set the error indicator of a stream.
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
#include "fseterr.h"

#include <errno.h>

void
fseterr (FILE *fp)
{
  /* Most systems provide FILE as a struct and the necessary bitmask in
     <stdio.h>, because they need it for implementing getc() and putc() as
     fast macros.  */
#if defined _IO_ferror_unlocked     /* GNU libc, BeOS */
  fp->_flags |= _IO_ERR_SEEN;
#elif defined __sferror             /* FreeBSD, NetBSD, OpenBSD, MacOS X, Cygwin */
  fp->_flags |= __SERR;
#elif defined _IOERR                /* AIX, HP-UX, IRIX, OSF/1, Solaris, mingw */
# if defined __sun && defined __sparc && defined _LP64 /* Solaris/SPARC 64-bit */
  ((unsigned int *) fp) [9] |= _IOERR;
# else
  fp->_flag |= _IOERR;
# endif
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
