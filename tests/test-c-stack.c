/* Test of c-stack module.
   Copyright (C) 2002, 2004, 2006, 2008 Free Software Foundation, Inc.

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

#include <config.h>

#include "c-stack.h"

#include "exitfail.h"
#include <stdio.h>
#include <stdlib.h>
#if HAVE_SETRLIMIT
/* At least FreeBSD 5.0 needs extra headers before <sys/resource.h>
   will compile.  */
# include <sys/types.h>
# include <sys/time.h>
# include <sys/resource.h>
#endif

#define ASSERT(expr) \
  do									     \
    {									     \
      if (!(expr))							     \
	{								     \
	  fprintf (stderr, "%s:%d: assertion failed\n", __FILE__, __LINE__); \
	  fflush (stderr);						     \
	  abort ();							     \
	}								     \
    }									     \
  while (0)

static long
recurse (char *p)
{
  char array[500];
  array[0] = 1;
  return *p + recurse (array);
}

char *program_name;

int
main (int argc, char **argv)
{
#if HAVE_SETRLIMIT && defined RLIMIT_STACK
  /* Before starting the endless recursion, try to be friendly to the
     user's machine.  On some Linux 2.2.x systems, there is no stack
     limit for user processes at all.  We don't want to kill such
     systems.  */
  struct rlimit rl;
  rl.rlim_cur = rl.rlim_max = 0x100000; /* 1 MB */
  setrlimit (RLIMIT_STACK, &rl);
#endif

  program_name = argv[0];
  if (c_stack_action (0) == 0)
    {
      if (1 < argc)
	{
	  exit_failure = 77;
	  ++*argv[argc]; /* Intentionally dereference NULL.  */
	}
      return recurse ("\1");
    }
  perror ("c_stack_action");
  return 77;
}
