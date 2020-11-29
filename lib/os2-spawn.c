/* Auxiliary functions for the creation of subprocesses.  OS/2 kLIBC API.
   Copyright (C) 2001, 2003-2020 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2003.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#include <config.h>

/* Specification.  */
#include "os2-spawn.h"

/* Get _open_osfhandle().  */
#include <io.h>

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "cloexec.h"
#include "error.h"
#include "xalloc.h"
#include "gettext.h"

#define _(str) gettext (str)


/* Duplicates a file handle, making the copy uninheritable.
   Returns -1 for a file handle that is equivalent to closed.  */
static int
dup_noinherit (int fd)
{
  fd = dup_cloexec (fd);
  if (fd < 0 && errno == EMFILE)
    error (EXIT_FAILURE, errno, _("_open_osfhandle failed"));

  return fd;
}

/* Returns a file descriptor equivalent to FD, except that the resulting file
   descriptor is none of STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO.
   FD must be open and non-inheritable.  The result will be non-inheritable as
   well.
   If FD < 0, FD itself is returned.  */
static int
fd_safer_noinherit (int fd)
{
  if (STDIN_FILENO <= fd && fd <= STDERR_FILENO)
    {
      /* The recursion depth is at most 3.  */
      int nfd = fd_safer_noinherit (dup_noinherit (fd));
      int saved_errno = errno;
      close (fd);
      errno = saved_errno;
      return nfd;
    }
  return fd;
}

int
dup_safer_noinherit (int fd)
{
  return fd_safer_noinherit (dup_noinherit (fd));
}

void
undup_safer_noinherit (int tempfd, int origfd)
{
  if (tempfd >= 0)
    {
      if (dup2 (tempfd, origfd) < 0)
        error (EXIT_FAILURE, errno, _("cannot restore fd %d: dup2 failed"),
               origfd);
      close (tempfd);
    }
  else
    {
      /* origfd was closed or open to no handle at all.  Set it to a closed
         state.  This is (nearly) equivalent to the original state.  */
      close (origfd);
    }
}

char **
prepare_spawn (char **argv)
{
  size_t argc;
  char **new_argv;
  size_t i;

  /* Count number of arguments.  */
  for (argc = 0; argv[argc] != NULL; argc++)
    ;

  /* Allocate new argument vector.  */
  new_argv = XNMALLOC (1 + argc + 1, char *);

  /* Add an element upfront that can be used when argv[0] turns out to be a
     script, not a program.
     On Unix, this would be "/bin/sh".  */
  *new_argv++ = "sh.exe";

  /* Put quoted arguments into the new argument vector.  */
  for (i = 0; i < argc; i++)
    {
      const char *string = argv[i];

      if (string[0] == '\0')
        new_argv[i] = xstrdup ("\"\"");
      else
        new_argv[i] = (char *) string;
    }
  new_argv[argc] = NULL;

  return new_argv;
}
