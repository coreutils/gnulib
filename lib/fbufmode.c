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
#include "fbufmode.h"

#if HAVE___FLBF
# include <stdio_ext.h>
#endif

int
fbufmode (FILE *fp)
{
  /* Most systems provide FILE as a struct and the necessary bitmask in
     <stdio.h>, because they need it for implementing getc() and putc() as
     fast macros.  */
#if defined _IO_ferror_unlocked     /* GNU libc, BeOS */
# if HAVE___FLBF                    /* glibc >= 2.2 */
  if (__flbf (fp))
    return _IOLBF;
# else
  if (fp->_flags & _IO_LINE_BUF)
    return _IOLBF;
# endif
  if (fp->_flags & _IO_UNBUFFERED)
    return _IONBF;
  return _IOFBF;
#elif defined __sferror             /* FreeBSD, NetBSD, OpenBSD, MacOS X, Cygwin */
  if (fp->_flags & __SLBF)
    return _IOLBF;
  if (fp->_flags & __SNBF)
    return _IONBF;
  return _IOFBF;
#elif defined __EMX__               /* emx+gcc */
  return fp->_flags & (_IOLBF | _IONBF | _IOFBF);
#elif defined _IOERR                /* AIX, HP-UX, IRIX, OSF/1, Solaris, OpenServer, mingw */
# if HAVE___FLBF                    /* Solaris >= 7 */
  if (__flbf (fp))
    return _IOLBF;
# else
  if (fp->_flag & _IOLBF)
    return _IOLBF;
# endif
# if defined __sun && defined _LP64 /* Solaris/{SPARC,AMD64} 64-bit */
#  define fp_ ((struct { unsigned char *_ptr; \
			 unsigned char *_base; \
			 unsigned char *_end; \
			 long _cnt; \
			 int _file; \
			 unsigned int _flag; \
		       } *) fp)
  return fp_->_flag & (_IONBF | _IOFBF);
# else
#  if defined _SCO_DS               /* OpenServer */
#   define _flag __flag
#  endif
  if (fp->_flag & _IONBF)
    return _IONBF;
  return _IOFBF;
# endif
#elif defined __UCLIBC__            /* uClibc */
  if (fp->__modeflags & __FLAG_LBF)
    return _IOLBF;
  if (fp->__modeflags & __FLAG_NBF)
    return _IONBF;
  return _IOFBF;
#elif defined __QNX__               /* QNX */
  if (fp->_Mode & 0x400 /* _MLBF */)
    return _IOLBF;
  if (fp->_Mode & 0x800 /* _MNBF */)
    return _IONBF;
  return _IOFBF;
#else
 #error "Please port gnulib fbufmode.c to your platform! Look at the setvbuf implementation."
#endif
}
