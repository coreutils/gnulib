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

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Return the buffering mode of the stream STREAM.

   Result    means
   ------   -------
   _IONBF  unbuffered
   _IOLBF  line buffered
   _IOFBF  fully buffered

   STREAM must not be wide-character oriented.  */
extern int fbufmode (FILE *stream);

#ifdef __cplusplus
}
#endif
