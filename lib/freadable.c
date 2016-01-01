/* Retrieve information about a FILE stream.
   Copyright (C) 2007-2016 Free Software Foundation, Inc.

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
#include "freadable.h"

#include "stdio-impl.h"

#if defined EPLAN9                  /* Plan9 */
# include <fcntl.h>
#endif

bool
freadable (FILE *fp)
{
  /* Most systems provide FILE as a struct and the necessary bitmask in
     <stdio.h>, because they need it for implementing getc() and putc() as
     fast macros.  */
#if defined _IO_ftrylockfile || __GNU_LIBRARY__ == 1 /* GNU libc, BeOS, Haiku, Linux libc5 */
  return (fp->_flags & _IO_NO_READS) == 0;
#elif defined __sferror || defined __DragonFly__ || defined __ANDROID__
  /* FreeBSD, NetBSD, OpenBSD, DragonFly, Mac OS X, Cygwin, Android */
  return (fp_->_flags & (__SRW | __SRD)) != 0;
#elif defined __EMX__               /* emx+gcc */
  return (fp->_flags & (_IORW | _IOREAD)) != 0;
#elif defined __minix               /* Minix */
  return (fp->_flags & _IOREAD) != 0;
#elif defined _IOERR                /* AIX, HP-UX, IRIX, OSF/1, Solaris, OpenServer, mingw, NonStop Kernel */
  return (fp->_flag & (_IORW | _IOREAD)) != 0;
#elif defined __QNX__               /* QNX */
  return (fp->_Mode & 0x1 /* _MOPENR */) != 0;
#elif defined __MINT__              /* Atari FreeMiNT */
  return fp->__mode.__read;
#elif defined EPLAN9                /* Plan9 */
  int fd = fp->fd;
  if (fd >= 0)
    {
      int flags = fcntl (fd, F_GETFL, NULL);
      if (flags >= 0)
        {
          flags &= O_ACCMODE;
          return (flags == O_RDONLY || flags == O_RDWR);
        }
    }
  return 0;
#else
# error "Please port gnulib freadable.c to your platform! Look at the definition of fopen, fdopen on your system, then report this to bug-gnulib."
#endif
}
