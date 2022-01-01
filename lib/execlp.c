/* execlp() function: Execute a program, replacing the current process.
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
   may optimize away the arg0 == NULL test below.  */
#define _GL_ARG_NONNULL(params)

#include <config.h>

/* Specification.  */
#include <unistd.h>

#include <errno.h>
#include <stdarg.h>

#include "malloca.h"

int
execlp (const char *program, const char *arg0, ...)
{
  va_list args;

  /* The callee is not expecting a NULL argv[0].  */
  if (arg0 == NULL)
    {
      errno = EINVAL;
      return -1;
    }

  /* Count the number of arguments (including arg0 and the trailing NULL).  */
  size_t count = 1;
  va_start (args, arg0);
  for (;;)
    {
      count++;
      if (va_arg (args, const char *) == NULL)
        break;
    }
  va_end (args);

  /* Allocate the argument vector.  */
  const char **argv = (const char **) malloca (count * sizeof (const char *));
  if (argv == NULL)
    {
      errno = ENOMEM;
      return -1;
    }

  /* Copy the arguments into the argument vector.  */
  {
    size_t i = 0;
    argv[i++] = arg0;
    va_start (args, arg0);
    for (; i < count;)
      argv[i++] = va_arg (args, const char *);
    va_end (args);
  }

  /* Invoke execvp.  */
  execvp (program, argv);

  /* If execvp returned, it must have failed.  */
  int saved_errno = errno;
  freea (argv);
  errno = saved_errno;
  return -1;
}
