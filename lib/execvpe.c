/* execvpe() function: Execute a program, replacing the current process.
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

#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>

#include "findprog.h"

int
execvpe (const char *program, char * const *argv, char * const *env)
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

  const char *resolved_progname =
    find_in_given_path (program, getenv ("PATH"), NULL, true);
  if (resolved_progname == NULL)
    return -1;

  /* Invoke execve.  */
  execve (resolved_progname, argv, env);

  /* If execve returned, it must have failed.  */
  if (resolved_progname != program)
    free ((char *) resolved_progname);
  return -1;
}
