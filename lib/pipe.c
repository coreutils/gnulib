/* Create a pipe.
   Copyright (C) 2009-2010 Free Software Foundation, Inc.

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

#include <config.h>

/* Specification.  */
#include <unistd.h>

#if (defined _WIN32 || defined __WIN32__) && ! defined __CYGWIN__
/* Native Woe32 API.  */

/* Get _pipe().  */
# include <io.h>

/* Get _O_BINARY.  */
# include <fcntl.h>

int
pipe (int fd[2])
{
  return _pipe (fd, 4096, _O_BINARY);
}

#else

# error "This platform lacks a pipe function, and Gnulib doesn't provide a replacement. This is a bug in Gnulib."

#endif
