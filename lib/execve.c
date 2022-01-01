/* execve() function: Execute a program, replacing the current process.
   Copyright (C) 2020-2022 Free Software Foundation, Inc.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of the
   License, or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Written by Bruno Haible <bruno@clisp.org>, 2020.  */

/* Don't use __attribute__ __nonnull__ in this compilation unit.  Otherwise gcc
   may optimize away the program == NULL and argv == NULL tests below.  */
#define _GL_ARG_NONNULL(params)

#include <config.h>

/* Specification.  */
#include <unistd.h>

#if defined _WIN32 && !defined __CYGWIN__

# include <errno.h>
# include <stdio.h>
# include <spawn.h>

# include <sys/types.h>
# include <sys/wait.h>

int
execve (const char *program, char * const *argv, char * const *env)
{
  if (program == NULL
      || argv == NULL
      /* The callee is not expecting a NULL argv[0].  */
      || argv[0] == NULL
      || env == NULL)
    {
      errno = EINVAL;
      return -1;
    }

  pid_t child;
  int err = posix_spawn (&child, program, NULL, NULL, argv, env);
  if (err == 0)
    {
      /* Close all file descriptors, so that
         - for opened files, the child may close and unlink() the file,
         - for pipe handles, the pipe's reader or writer does not get stuck.
         Ideally we would also suspend all other threads and close all other
         HANDLEs (not associated with file descriptors) and SOCKETs, but we
         can't do so since we have no way to enumerate them.  */
      {
        unsigned int fdmax = _getmaxstdio ();
        unsigned int fd;
        for (fd = 0; fd < fdmax; fd++)
          close (fd);
      }

      /* Wait until the child process is terminated.  */
      int status = 127;
      waitpid (child, &status, 0);
      int exitcode = status;

      /* Pass its exit code to the parent process.  */
      _exit (exitcode);
    }
  errno = err;
  return -1;
}

#endif
