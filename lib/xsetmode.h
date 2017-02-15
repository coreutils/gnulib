/* Binary mode I/O with checking
   Copyright 2017 Free Software Foundation, Inc.

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

#ifndef _XSETMODE_H
#define _XSETMODE_H

#include "binary-io.h"

#ifndef _GL_INLINE_HEADER_BEGIN
 #error "Please include config.h first."
#endif
_GL_INLINE_HEADER_BEGIN
#ifndef XSETMODE_INLINE
# define XSETMODE_INLINE _GL_INLINE
#endif

#if O_BINARY
extern _Noreturn void xsetmode_error (void);
#else
XSETMODE_INLINE void xsetmode_error (void) {}
#endif

XSETMODE_INLINE void
xsetmode (int fd, int mode)
{
  if (set_binary_mode (fd, mode) < 0)
    xsetmode_error ();
}

_GL_INLINE_HEADER_END

#endif /* _XSETMODE_H */
