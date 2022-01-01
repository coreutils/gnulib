/* Acquire ownership of the slave side of a pseudo-terminal.
   Copyright (C) 1998-2002, 2009-2022 Free Software Foundation, Inc.
   Contributed by Zack Weinberg <zack@rabi.phys.columbia.edu>, 1998.

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

#include <config.h>

#include <stdlib.h>

#include <assert.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#if HAVE_SETRLIMIT
# include <sys/types.h>
# include <sys/time.h>
# include <sys/resource.h>
#endif

#include "configmake.h"
#include "pty-private.h"

#ifndef _LIBC
# define __set_errno(e) errno = (e)
# define __dup2 dup2
# define __fork fork
# define __setrlimit setrlimit
# define __waitpid waitpid
#endif

int
grantpt (int fd)
{
#if defined __OpenBSD__ || defined _WIN32
  /* On OpenBSD, master and slave of a pseudo-terminal are allocated together,
     through an ioctl on /dev/ptm.  On Windows there are no ptys.
     Therefore in either case there is no need for grantpt().  */
  return 0;
#else
  /* This function is most often called from a process without 'root'
     credentials.  Use the helper program.  */
  int retval = -1;
  pid_t pid = __fork ();
  if (pid == -1)
    goto cleanup;
  else if (pid == 0)
    {
      /* This is executed in the child process.  */

# if HAVE_SETRLIMIT && defined RLIMIT_CORE
      /* Disable core dumps.  */
      struct rlimit rl = { 0, 0 };
      __setrlimit (RLIMIT_CORE, &rl);
# endif

      /* We pass the master pseudo terminal as file descriptor PTY_FILENO.  */
      if (fd != PTY_FILENO)
        if (__dup2 (fd, PTY_FILENO) < 0)
          _exit (FAIL_EBADF);

# ifdef CLOSE_ALL_FDS
      CLOSE_ALL_FDS ();
# endif

      execle (_PATH_PT_CHOWN, strrchr (_PATH_PT_CHOWN, '/') + 1, NULL, NULL);
      _exit (FAIL_EXEC);
    }
  else
    {
      int w;

      if (__waitpid (pid, &w, 0) == -1)
        goto cleanup;
      if (!WIFEXITED (w))
        __set_errno (ENOEXEC);
      else
        switch (WEXITSTATUS (w))
          {
          case 0:
            retval = 0;
            break;
          case FAIL_EBADF:
            __set_errno (EBADF);
            break;
          case FAIL_EINVAL:
            __set_errno (EINVAL);
            break;
          case FAIL_EACCES:
            __set_errno (EACCES);
            break;
          case FAIL_EXEC:
            __set_errno (ENOEXEC);
            break;
          case FAIL_ENOMEM:
            __set_errno (ENOMEM);
            break;

          default:
            assert(! "getpt: internal error: invalid exit code from pt_chown");
          }
    }

 cleanup:
  return retval;
#endif
}
