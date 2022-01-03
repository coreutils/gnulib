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
