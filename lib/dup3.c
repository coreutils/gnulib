/* Copy a file descriptor, applying specific flags.
   Copyright (C) 2009-2011 Free Software Foundation, Inc.

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

#include <errno.h>
#include <fcntl.h>
#include <limits.h>

#include "binary-io.h"

#if (defined _WIN32 || defined __WIN32__) && ! defined __CYGWIN__
/* Native Woe32 API.  */

# include <string.h>

/* Get declarations of the Win32 API functions.  */
# define WIN32_LEAN_AND_MEAN
# include <windows.h>

/* Upper bound on getdtablesize().  See lib/getdtablesize.c.  */
# define OPEN_MAX_MAX 0x10000

#endif

int
dup3 (int oldfd, int newfd, int flags)
{
#if HAVE_DUP3
# undef dup3
  /* Try the system call first, if it exists.  (We may be running with a glibc
     that has the function but with an older kernel that lacks it.)  */
  {
    /* Cache the information whether the system call really exists.  */
    static int have_dup3_really; /* 0 = unknown, 1 = yes, -1 = no */
    if (have_dup3_really >= 0)
      {
        int result = dup3 (oldfd, newfd, flags);
        if (!(result < 0 && errno == ENOSYS))
          {
            have_dup3_really = 1;
# if REPLACE_FCHDIR
            if (0 <= result)
              result = _gl_register_dup (oldfd, newfd);
# endif
            return result;
          }
        have_dup3_really = -1;
      }
  }
#endif

  if (newfd < 0 || newfd >= getdtablesize () || fcntl (oldfd, F_GETFD) == -1)
    {
      errno = EBADF;
      return -1;
    }

  if (newfd == oldfd)
    {
      errno = EINVAL;
      return -1;
    }

  /* Check the supported flags.
     Note that O_NONBLOCK is not supported, because setting it on newfd
     would implicitly also set it on oldfd.  */
  if ((flags & ~(O_CLOEXEC | O_BINARY | O_TEXT)) != 0)
    {
      errno = EINVAL;
      return -1;
    }

  if (flags & O_CLOEXEC)
    {
      int result;
      close (newfd);
      result = fcntl (oldfd, F_DUPFD_CLOEXEC, newfd);
      if (newfd < result)
        {
          close (result);
          errno = EIO;
          result = -1;
        }
      if (result < 0)
        return -1;
    }
  else if (dup2 (oldfd, newfd) < 0)
    return -1;

#if O_BINARY
  if (flags & O_BINARY)
    setmode (newfd, O_BINARY);
  else if (flags & O_TEXT)
    setmode (newfd, O_TEXT);
#endif

  return newfd;
}
