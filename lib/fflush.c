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

/* Specification.  */
#include <stdio.h>

#include <errno.h>
#include <unistd.h>

#include "freading.h"
#include "fpurge.h"

#undef fflush

/* Flush all pending data on STREAM according to POSIX rules.  Both
   output and seekable input streams are supported.  */
int
rpl_fflush (FILE *stream)
{
  int result;
  off_t pos;

  /* When stream is NULL, POSIX and C99 only require flushing of "output
     streams and update streams in which the most recent operation was not
     input", and all implementations do this.

     When stream is "an output stream or an update stream in which the most
     recent operation was not input", POSIX and C99 requires that fflush
     writes out any buffered data, and all implementations do this.

     When stream is, however, an input stream or an update stream in
     which the most recent operation was input, C99 specifies nothing,
     and POSIX only specifies behavior if the stream is seekable.
     mingw, in particular, drops the input buffer, leaving the file
     descriptor positioned at the end of the input buffer. I.e. ftell
     (stream) is lost.  We don't want to call the implementation's
     fflush in this case.

     We test ! freading (stream) here, rather than fwriting (stream), because
     what we need to know is whether the stream holds a "read buffer", and on
     mingw this is indicated by _IOREAD, regardless of _IOWRT.  */
  if (stream == NULL || ! freading (stream))
    return fflush (stream);

  /* POSIX does not specify fflush behavior for non-seekable input
     streams.  Some implementations purge unread data, some return
     EBADF, some do nothing.  */
  pos = ftello (stream);
  if (pos == -1)
    {
      errno = EBADF;
      return EOF;
    }

  /* To get here, we must be flushing a seekable input stream, so the
     semantics of fpurge are now appropriate to clear the buffer.  To
     avoid losing data, the lseek is also necessary.  */
  result = fpurge (stream);
  if (result != 0)
    return result;

  pos = lseek (fileno (stream), pos, SEEK_SET);
  if (pos == -1)
    return EOF;
  /* After a successful lseek, update the file descriptor's position cache
     in the stream.  */
#if defined __sferror           /* FreeBSD, NetBSD, OpenBSD, MacOS X, Cygwin */
  stream->_offset = pos;
  stream->_flags |= __SOFF;
#endif

  return 0;
}
