/* Open a stream to a sub-process.
   Copyright (C) 2009 Free Software Foundation, Inc.

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

/* Written by Eric Blake <ebb9@byu.net>, 2009.  */

#include <config.h>

/* Get the original definition of popen.  It might be defined as a macro.  */
#define __need_FILE
# include <stdio.h>
#undef __need_FILE

static inline FILE *
orig_popen (const char *filename, const char *mode)
{
  return popen (filename, mode);
}

/* Specification.  */
#include <stdio.h>

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

FILE *
rpl_popen (const char *filename, const char *mode)
{
  /* The mingw popen works fine, and all other platforms have fcntl.
     The bug of the child clobbering its own file descriptors if stdin
     or stdout was closed in the parent can be worked around by
     opening those two fds as close-on-exec to begin with.  */
  /* Cygwin 1.5.x also has a bug where the popen fd is improperly
     marked close-on-exec, and if the application undoes this, then
     the fd leaks into subsequent popen calls.  We could work around
     this by maintaining a list of all fd's opened by popen, and
     temporarily marking them cloexec around the real popen call, but
     we would also have to override pclose, and the bookkeepping seems
     extreme given that cygwin 1.7 no longer has the bug.  */
  FILE *result;
  int cloexec0 = fcntl (STDIN_FILENO, F_GETFD);
  int cloexec1 = fcntl (STDOUT_FILENO, F_GETFD);
  int saved_errno;

  if (cloexec0 < 0)
    {
      if (open ("/dev/null", O_RDONLY) != STDIN_FILENO
	  || fcntl (STDIN_FILENO, F_SETFD,
		    fcntl (STDIN_FILENO, F_GETFD) | FD_CLOEXEC) == -1)
	abort ();
    }
  if (cloexec1 < 0)
    {
      if (open ("/dev/null", O_RDONLY) != STDOUT_FILENO
	  || fcntl (STDOUT_FILENO, F_SETFD,
		    fcntl (STDOUT_FILENO, F_GETFD) | FD_CLOEXEC) == -1)
	abort ();
    }
  result = orig_popen (filename, mode);
  saved_errno = errno;
  if (cloexec0 < 0)
    close (STDIN_FILENO);
  if (cloexec1 < 0)
    close (STDOUT_FILENO);
  errno = saved_errno;
  return result;
}
