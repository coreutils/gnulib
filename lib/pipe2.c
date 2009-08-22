/* Create a pipe, with specific opening flags.
   Copyright (C) 2009 Free Software Foundation, Inc.

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

#if (defined _WIN32 || defined __WIN32__) && ! defined __CYGWIN__
/* Native Woe32 API.  */

# include <io.h>

int
pipe2 (int fd[2], int flags)
{
  /* Check the supported flags.  */
  if ((flags & ~(O_CLOEXEC | O_BINARY | O_TEXT)) != 0)
    {
      errno = EINVAL;
      return -1;
    }

  return _pipe (fd, 4096, flags);
}

#else
/* Unix API.  */

# ifndef O_CLOEXEC
#  define O_CLOEXEC 0
# endif

int
pipe2 (int fd[2], int flags)
{
  /* Check the supported flags.  */
  if ((flags & ~(O_CLOEXEC | O_NONBLOCK)) != 0)
    {
      errno = EINVAL;
      return -1;
    }

  if (pipe (fd) < 0)
    return -1;

  /* POSIX <http://www.opengroup.org/onlinepubs/9699919799/functions/pipe.html>
     says that initially, the O_NONBLOCK and FD_CLOEXEC flags are cleared on
     both fd[0] amd fd[1].  */

  if (flags & O_NONBLOCK)
    {
      int fcntl_flags;

      if ((fcntl_flags = fcntl (fd[1], F_GETFL, 0)) < 0
	  || fcntl (fd[1], F_SETFL, fcntl_flags | O_NONBLOCK) < 0
	  || (fcntl_flags = fcntl (fd[0], F_GETFL, 0)) < 0
	  || fcntl (fd[0], F_SETFL, fcntl_flags | O_NONBLOCK) < 0)
	goto fail;
    }

  if (flags & O_CLOEXEC)
    {
      int fcntl_flags;

      if ((fcntl_flags = fcntl (fd[1], F_GETFD, 0)) < 0
	  || fcntl (fd[1], F_SETFD, fcntl_flags | FD_CLOEXEC) < 0
	  || (fcntl_flags = fcntl (fd[0], F_GETFD, 0)) < 0
	  || fcntl (fd[0], F_SETFD, fcntl_flags | FD_CLOEXEC) < 0)
	goto fail;
    }

  return 0;

 fail:
  {
    int saved_errno = errno;
    close (fd[0]);
    close (fd[1]);
    errno = saved_errno;
    return -1;
  }
}

#endif
