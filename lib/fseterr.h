/* Set the error indicator of a stream.
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

#ifndef _FSETERR_H
#define _FSETERR_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif


/* Set the error indicator of the stream FP.
   The "error indicator" is set when an I/O operation on the stream fails, and
   is cleared (together with the "end-of-file" indicator) by clearerr (FP).  */
extern void fseterr (FILE *fp);


#ifdef __cplusplus
}
#endif

#endif /* _FSETERR_H */
