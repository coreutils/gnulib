/* Test of link() function.
   Copyright (C) 2009 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include <config.h>

#include <unistd.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

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
main (int argc, char **argv)
{
  int ret;

  ASSERT (argc == 3);

  ret = link (argv[1], argv[2]);
  if (ret < 0)
    {
      /* If the device does not support hard links, errno is
	 EPERM on Linux, EOPNOTSUPP on FreeBSD.  */
      switch (errno)
	{
	case EPERM:
#ifdef EOPNOTSUPP
	case EOPNOTSUPP:
#endif
	  return 77;
	default:
	  perror ("link");
	  return 1;
	}
    }

  return 0;
}
