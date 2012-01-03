/* isatty() replacement.
   Copyright (C) 2012 Free Software Foundation, Inc.

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

#include <config.h>

/* Specification.  */
#include <unistd.h>

/* This replacement is enabled on native Windows.  */

#include <errno.h>

/* Get declarations of the Win32 API functions.  */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "msvc-inval.h"

/* Get _get_osfhandle().  */
#include "msvc-nothrow.h"

/* Optimized test whether a HANDLE refers to a console.
   See <http://lists.gnu.org/archive/html/bug-gnulib/2009-08/msg00065.html>.  */
#define IsConsoleHandle(h) (((long) (h) & 3) == 3)

#if HAVE_MSVC_INVALID_PARAMETER_HANDLER
static inline int
_isatty_nothrow (int fd)
{
  int result;

  TRY_MSVC_INVAL
    {
      result = _isatty (fd);
    }
  CATCH_MSVC_INVAL
    {
      result = -1;
      errno = EBADF;
    }
  DONE_MSVC_INVAL;

  return result;
}
#else
# define _isatty_nothrow _isatty
#endif

int
isatty (int fd)
{
  /* _isatty (fd) tests whether GetFileType of the handle is FILE_TYPE_CHAR.  */
  if (_isatty_nothrow (fd))
    {
      HANDLE h = (HANDLE) _get_osfhandle (fd);
      return IsConsoleHandle (h);
    }
  else
    return 0;
}
