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
  fp->_flag |= _IOERR;
#else                               /* unknown  */
  /* Portable fallback, based on an idea by Rich Felker.
     Wow! 6 system calls for something that is just a bit operation!  */
  int fd;
  int fd2;

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
#endif
}
