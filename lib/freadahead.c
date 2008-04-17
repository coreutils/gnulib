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

size_t
freadahead (FILE *fp)
{
#if defined _IO_ferror_unlocked || __GNU_LIBRARY__ == 1 /* GNU libc, BeOS, Linux libc5 */
  if (fp->_IO_write_ptr > fp->_IO_write_base)
    return 0;
  return (fp->_IO_read_end - fp->_IO_read_ptr)
	 + (fp->_flags & _IO_IN_BACKUP ? fp->_IO_save_end - fp->_IO_save_base :
	    0);
#elif defined __sferror             /* FreeBSD, NetBSD, OpenBSD, MacOS X, Cygwin */
# if defined __NetBSD__ || defined __OpenBSD__
  struct __sfileext
    {
      struct  __sbuf _ub; /* ungetc buffer */
      /* More fields, not relevant here.  */
    };
# define HASUB(fp) (((struct __sfileext *) (fp)->_ext._base)->_ub._base != NULL)
# else
# define HASUB(fp) ((fp)->_ub._base != NULL)
# endif
  if ((fp->_flags & __SWR) != 0 || fp->_r < 0)
    return 0;
  return fp->_r
	 + (HASUB (fp) ? fp->_ur : 0);
#elif defined __EMX__               /* emx+gcc */
  if ((fp->_flags & _IOWRT) != 0)
    return 0;
  /* Note: fp->_ungetc_count > 0 implies fp->_rcount <= 0,
           fp->_ungetc_count = 0 implies fp->_rcount >= 0.  */
  /* equivalent to
     (fp->_ungetc_count == 0 ? fp->_rcount : fp->_ungetc_count - fp->_rcount) */
  return (fp->_rcount > 0 ? fp->_rcount : fp->_ungetc_count - fp->_rcount);
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
    return 0;
  return fp_->_cnt;
# else
#  if defined _SCO_DS               /* OpenServer */
#   define _flag __flag
#   define _cnt __cnt
#  endif
  if ((fp->_flag & _IOWRT) != 0)
    return 0;
  return fp->_cnt;
# endif
#elif defined __UCLIBC__            /* uClibc */
# ifdef __STDIO_BUFFERS
  if (fp->__modeflags & __FLAG_WRITING)
    return 0;
  return (fp->__bufread - fp->__bufpos)
	 + (fp->__modeflags & __FLAG_UNGOT ? 1 : 0);
# else
  return 0;
# endif
#elif defined __QNX__               /* QNX */
  if ((fp->_Mode & 0x2000 /* _MWRITE */) != 0)
    return 0;
  /* fp->_Buf <= fp->_Next <= fp->_Rend,
     and fp->_Rend may be overridden by fp->_Rsave. */
  return ((fp->_Rsave ? fp->_Rsave : fp->_Rend) - fp->_Next)
	 + (fp->_Mode & 0x4000 /* _MBYTE */
	    ? (fp->_Back + sizeof (fp->_Back)) - fp->_Rback
	    : 0);
#else
 #error "Please port gnulib freadahead.c to your platform! Look at the definition of fflush, fread, ungetc on your system, then report this to bug-gnulib."
#endif
}
