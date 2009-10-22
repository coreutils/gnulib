/* Test that openat works.
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

/* Written by Eric Blake <ebb9@byu.net>, 2009.  */

#include <config.h>

#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define ASSERT(expr) \
  do                                                                         \
    {                                                                        \
      if (!(expr))                                                           \
	{                                                                    \
	  fprintf (stderr, "%s:%d: assertion failed\n", __FILE__, __LINE__); \
	  fflush (stderr);                                                   \
	  abort ();                                                          \
	}                                                                    \
    }                                                                        \
  while (0)

int
main (void)
{
  /* FIXME - add more tests.  For example, share /dev/null and
     trailing slash tests with test-open, and do more checks for
     proper fd handling.  */

  /* Check that even when *-safer modules are in use, plain openat can
     land in fd 0.  Do this test last, since it is destructive to
     stdin.  */
  ASSERT (close (STDIN_FILENO) == 0);
  ASSERT (openat (AT_FDCWD, ".", O_RDONLY) == STDIN_FILENO);
  {
    int dfd = open (".", O_RDONLY);
    ASSERT (STDIN_FILENO < dfd);
    ASSERT (chdir ("..") == 0);
    ASSERT (close (STDIN_FILENO) == 0);
    ASSERT (openat (dfd, ".", O_RDONLY) == STDIN_FILENO);
    ASSERT (close (dfd) == 0);
  }
  return 0;
}
