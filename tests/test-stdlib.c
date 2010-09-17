/* Test of <stdlib.h> substitute.
   Copyright (C) 2007, 2009, 2010 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2007.  */

#include <config.h>

#include <stdlib.h>

#include "verify.h"

/* Check that EXIT_SUCCESS is 0, per POSIX.  */
static int exitcode = EXIT_SUCCESS;
#if EXIT_SUCCESS
"oops"
#endif

/* Check for GNU value (not guaranteed by POSIX, but is guaranteed by
   gnulib).  */
#if EXIT_FAILURE != 1
"oops"
#endif

/* Check that NULL can be passed through varargs as a pointer type,
   per POSIX 2008.  */
verify (sizeof NULL == sizeof (void *));

int
main (void)
{
  /* Check subset of <sys/wait.h> macros that must be visible here.
     Note that some of these macros are only portable when operating
     on an lvalue.  */
  int i;
  for (i = 0; i < 0x10000; i = (i ? i << 1 : 1))
    if (!!WIFSIGNALED (i) + !!WIFEXITED (i) + !!WIFSTOPPED (i) != 1)
      return 1;
  i = WEXITSTATUS (i) + WSTOPSIG (i) + WTERMSIG (i);

  switch (i)
    {
#if 0
  /* Gnulib doesn't guarantee these, yet.  */
    case WNOHANG:
    case WUNTRACED:
#endif
      break;
    }
  return exitcode;
}
