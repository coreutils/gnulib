/* execv() function: Execute a program, replacing the current process.
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

#include <config.h>

/* Specification.  */
#include <unistd.h>

int
execv (const char *program, char * const *argv)
{
  /* Pass the environment explicitly.  This is needed if the program has
     modified the environment using putenv() or [un]setenv().  On Windows,
     processes have two environments, one in the "environment block" of the
     process and managed through SetEnvironmentVariable(), and one inside the
     process, in the location retrieved by the 'environ' macro.  If we were
     to pass NULL, the child process would inherit a copy of the environment
     block - ignoring the effects of putenv() and [un]setenv().  */
  return execve (program, argv, environ);
}
