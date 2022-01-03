/* Open a stream to a sub-process.
   Copyright (C) 2009-2022 Free Software Foundation, Inc.

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

/* Written by Eric Blake <ebb9@byu.net>, 2009.  */

#include <config.h>

/* Specification.  */
#include <stdio.h>

#if defined _WIN32 && ! defined __CYGWIN__
/* Native Windows API.  */

# include <string.h>

FILE *
popen (const char *filename, const char *mode)
{
  /* Use binary mode by default.  */
  if (strcmp (mode, "r") == 0)
    mode = "rb";
  else if (strcmp (mode, "w") == 0)
    mode = "wb";

  return _popen (filename, mode);
}

#else

# include <errno.h>
# include <fcntl.h>
# include <stdlib.h>
# include <unistd.h>

# undef popen

FILE *
rpl_popen (const char *filename, const char *mode)
{
  /* All other platforms have popen and fcntl.
     The bug of the child clobbering its own file descriptors if stdin
     or stdout was closed in the parent can be worked around by
     opening those two fds as close-on-exec to begin with.  */
  /* Cygwin 1.5.x also has a bug where the popen fd is improperly
     marked close-on-exec, and if the application undoes this, then
     the fd leaks into subsequent popen calls.  We could work around
     this by maintaining a list of all fd's opened by popen, and
     temporarily marking them cloexec around the real popen call, but
     we would also have to override pclose, and the bookkeeping seems
     extreme given that cygwin 1.7 no longer has the bug.  */
  FILE *result;
  int cloexec0 = fcntl (STDIN_FILENO, F_GETFD);
  int cloexec1 = fcntl (STDOUT_FILENO, F_GETFD);
  int saved_errno;

  /* If either stdin or stdout was closed (that is, fcntl failed),
     then we open a dummy close-on-exec fd to occupy that slot.  That
     way, popen's internal use of pipe() will not contain either fd 0
     or 1, overcoming the fact that the child process blindly calls
     close() on the parent's end of the pipe without first checking
     whether it is clobbering the fd just placed there via dup2(); the
     exec will get rid of the dummy fd's in the child.  Fortunately,
     closed stderr in the parent does not cause problems in the
     child.  */
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
  result = popen (filename, mode);
  /* Now, close any dummy fd's created in the parent.  */
  saved_errno = errno;
  if (cloexec0 < 0)
    close (STDIN_FILENO);
  if (cloexec1 < 0)
    close (STDOUT_FILENO);
  errno = saved_errno;
  return result;
}

#endif
