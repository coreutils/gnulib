/* Retrieve information about a FILE stream.
   Copyright (C) 2007, 2009-2022 Free Software Foundation, Inc.

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

#include <stdbool.h>
#include <stdio.h>

/* Return true if the stream STREAM supports writing, false if it supports
   only reading.
   STREAM must not be wide-character oriented.
   The result doesn't change until the stream is closed or re-opened.  */

#if HAVE___FWRITABLE
/* glibc >= 2.2, Solaris >= 7, UnixWare >= 7.1.4.MP4, Cygwin >= 1.7.34, Android API >= 23, musl libc */

# if HAVE_STDIO_EXT_H
#  include <stdio_ext.h>
# endif
# define fwritable(stream) (__fwritable (stream) != 0)

#else

# ifdef __cplusplus
extern "C" {
# endif

extern bool fwritable (FILE *stream) _GL_ATTRIBUTE_PURE;

# ifdef __cplusplus
}
# endif

#endif
