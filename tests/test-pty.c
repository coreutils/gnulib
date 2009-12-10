/* Test of pty.h and openpty/forkpty functions.
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

/* Written by Simon Josefsson <simon@josefsson.org>, 2009.  */

#include <config.h>

#include <pty.h>

#include <stdio.h>

int
main ()
{
  int res;
  int amaster;
  int aslave;

  res = openpty (&amaster, &aslave, NULL, NULL, NULL);
  if (res != 0)
    {
      printf ("openpty returned %d\n", res);
      return 1;
    }

  res = forkpty (&amaster, NULL, NULL, NULL);
  if (res == 0)
    {
      /* child process */
    }
  else if (res > 0)
    {
      /* parent */
    }
  else
    {
      printf ("forkpty returned %d\n", res);
      return 1;
    }

  return 0;
}
