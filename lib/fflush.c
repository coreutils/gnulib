/* fflush.c -- allow flushing input streams
   Copyright (C) 2007 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

/* Written by Eric Blake. */

#include <config.h>
#include <errno.h>
#include <stdio.h>

#if HAVE_STDIO_EXT_H
# include <stdio_ext.h>
#endif

#if HAVE_FPURGE && ! HAVE_DECL_FPURGE
int fpurge (FILE *);
#endif

#undef fflush

/* Flush all pending data on STREAM according to POSIX rules.  Both
   output and seekable input streams are supported.  */
int
rpl_fflush (FILE *stream)
{
  int e1; /* Leave errno unchanged on success.  */
  int e2; /* Capture errno of first fflush if nothing else succeeds.  */
  int result;

  /* Try flushing the stream.  C89 guarantees behavior of output
     streams, so we only need to worry if failure might have been on
     an input stream.  When stream is NULL, POSIX only requires
     flushing of output streams.  */
  e1 = errno;
  result = fflush (stream);
  if (! stream || result == 0 || errno != EBADF)
    return result;

  /* POSIX does not specify behavior for non-seekable streams.  */
  e2 = errno;
  if (fseeko (stream, 0, SEEK_CUR) != 0)
    {
      errno = e2;
      return EOF;
    }

  /* To get here, we must be flushing a seekable input stream, so the
     semantics of fpurge are now appropriate.  */
#if HAVE_FPURGE
  errno = e1;
  result = fpurge (stream);
#elif HAVE___FPURGE
  /* __fpurge has no return value, and on Solaris, we can't even trust
     errno.  So assume it succeeds.  */
  __fpurge (stream);
  result = 0;
  errno = e1;
#else /* ! HAVE___FPURGE */

  /* No single replacement; do it manually.  */
  {
    off_t position = ftello (stream);
    if (position == -1)
      {
	result = EOF; /* Should not happen; we know stream is seekable.  */
      }
    /* Set position of stream; hopefully the stdio routines don't
       overoptimize by not setting the underlying file position.  */
    else if (fseeko (stream, position, SEEK_SET) != 0)
      {
	result = EOF;
	errno = e2;
      }
    else
      {
	result = 0;
	errno = e1;
      }
  }
#endif /* ! HAVE___FPURGE */

  return result;
}
