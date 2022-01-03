/* Retrieve information about a FILE stream.
   Copyright (C) 2007-2022 Free Software Foundation, Inc.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#include <config.h>

/* Specification.  */
#include "fbufmode.h"

#if HAVE___FLBF
# if HAVE_STDIO_EXT_H
#  include <stdio_ext.h>
# endif
#endif

#include "stdio-impl.h"

int
fbufmode (FILE *fp)
{
  /* Most systems provide FILE as a struct and the necessary bitmask in
     <stdio.h>, because they need it for implementing getc() and putc() as
     fast macros.  */
#if defined _IO_EOF_SEEN || defined _IO_ftrylockfile || __GNU_LIBRARY__ == 1
  /* GNU libc, BeOS, Haiku, Linux libc5 */
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
#elif defined __sferror || defined __DragonFly__ || defined __ANDROID__
  /* FreeBSD, NetBSD, OpenBSD, DragonFly, Mac OS X, Cygwin, Minix 3, Android */
  if (fp_->_flags & __SLBF)
    return _IOLBF;
  if (fp_->_flags & __SNBF)
    return _IONBF;
  return _IOFBF;
#elif defined __EMX__               /* emx+gcc */
  return fp->_flags & (_IOLBF | _IONBF | _IOFBF);
#elif defined __minix               /* Minix */
  return fp->_flags & (_IOLBF | _IONBF | _IOFBF);
#elif defined _IOERR                /* AIX, HP-UX, IRIX, OSF/1, Solaris, OpenServer, UnixWare, mingw, MSVC, NonStop Kernel, OpenVMS */
# if defined WINDOWS_OPAQUE_FILE
  if (fp_->_flag & 0x100)
    return _IOFBF; /* Impossible to distinguish _IOFBF and _IOLBF.  */
  else
    return _IONBF;
# else
#  if HAVE___FLBF                   /* Solaris >= 7 */
  if (__flbf (fp))
    return _IOLBF;
#  else
  if (fp_->_flag & _IOLBF)
    return _IOLBF;
#  endif
  if (fp_->_flag & _IONBF)
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
#elif defined __MINT__              /* Atari FreeMiNT */
  if (fp->__linebuf)
    return _IOLBF;
  return (fp->__bufsize > 0 ? _IOFBF : _IONBF);
#elif HAVE___FLBF && HAVE___FBUFSIZE /* musl libc */
  if (__flbf (fp))
    return _IOLBF;
  return (__fbufsize (fp) > 0 ? _IOFBF : _IONBF);
#elif defined EPLAN9                /* Plan9 */
  if (fp->flags & 2 /* LINEBUF */)
    return _IOLBF;
  if (fp->bufl)
    return _IOFBF;
  return _IONBF;
#else
# error "Please port gnulib fbufmode.c to your platform! Look at the setvbuf implementation."
#endif
}
