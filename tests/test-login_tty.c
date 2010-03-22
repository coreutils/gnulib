/* Test of login_tty() function.
   Copyright (C) 2010 Free Software Foundation, Inc.

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

/* Specification.  */
extern int login_tty (int);

#include <pty.h>
#include <stdio.h>

int
main ()
{
  int master;
  int slave;

  /* Open a pseudo-terminal, as a master-slave pair.  */
  {
    int res = openpty (&master, &slave, NULL, NULL, NULL);
    if (res != 0)
      {
        fprintf (stderr, "openpty returned %d\n", res);
        return 1;
      }
  }

  /* Create a new session and make it the controlling tty of this session.  */
  {
    int res = login_tty (slave);
    if (res < 0)
      {
        fprintf (stderr, "login_tty failed\n");
        return 1;
      }
  }

  return 0;
}
