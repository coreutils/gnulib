/* Retrieve information about a FILE stream.
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

#include <stdbool.h>
#include <stdio.h>

/* Return true if the stream STREAM is opened write-only or append-only, or
   if the last operation on the stream was a write operation.  Return false
   if the stream supports reading and the last operation on it was a read
   operation or there was no such operation.
   STREAM must not be wide-character oriented.  */

#if HAVE___FWRITING /* glibc >= 2.2, Solaris >= 7 */

# include <stdio_ext.h>
# define fwriting(stream) (__fwriting (stream) != 0)

#else

# ifdef __cplusplus
extern "C" {
# endif

extern bool fwriting (FILE *stream);

# ifdef __cplusplus
}
# endif

#endif
