/* Retrieve information about a FILE stream.
   Copyright (C) 2007, 2009-2014 Free Software Foundation, Inc.

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

#include <stdbool.h>
#include <stdio.h>

/* Return true if the stream STREAM supports reading, false if it supports
   only writing, i.e. if it was opened write-only or append-only.
   STREAM must not be wide-character oriented.
   The result doesn't change until the stream is closed or re-opened.  */

#if HAVE___FREADABLE /* glibc >= 2.2, Solaris >= 7, musl libc */

# include <stdio_ext.h>
# define freadable(stream) (__freadable (stream) != 0)

#else

# ifdef __cplusplus
extern "C" {
# endif

extern bool freadable (FILE *stream);

# ifdef __cplusplus
}
# endif

#endif
