/* Test of command line argument processing.
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

/* Written by Bruno Haible <bruno@clisp.org>, 2009.  */

#include <config.h>

#if GNULIB_GETOPT_GNU
# include <getopt.h>
#endif
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

/* The glibc/gnulib implementation of getopt supports setting optind = 0,
   but other implementations don't.  */
#if defined __GETOPT_PREFIX || (__GLIBC__ >= 2)
# define OPTIND_MIN 0
#else
# define OPTIND_MIN 1
#endif

#include "test-getopt.h"
#if GNULIB_GETOPT_GNU
# include "test-getopt_long.h"
#endif

int
main (void)
{
  unsetenv ("POSIXLY_CORRECT");

  test_getopt ();
#if GNULIB_GETOPT_GNU
  test_getopt_long ();

  setenv ("POSIXLY_CORRECT", "1", 0);
  test_getopt_long_posix ();
#endif

  return 0;
}
