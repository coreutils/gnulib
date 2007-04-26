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

#include "fpurge.h"

#undef fflush

/* Flush all pending data on STREAM according to POSIX rules.  Both
   output and seekable input streams are supported.  */
int
rpl_fflush (FILE *stream)
{
  int result;
  off_t pos;

  /* Try flushing the stream.  C89 guarantees behavior of output
     streams, so we only need to worry if failure might have been on
     an input stream.  When stream is NULL, POSIX only requires
     flushing of output streams.  */
  result = fflush (stream);
  if (! stream || result == 0 || errno != EBADF)
    return result;

  /* POSIX does not specify fflush behavior for non-seekable input
     streams.  */
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
#if defined __sferror               /* FreeBSD, NetBSD, OpenBSD, MacOS X, Cygwin */
  stream->_offset = pos;
  stream->_flags |= __SOFF;
#endif
  return 0;
}
