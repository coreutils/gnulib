/* Test of <stdnoreturn.h> and _Noreturn.
   Copyright 2011 Free Software Foundation, Inc.

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

/* written by Paul Eggert */

#include <config.h>

#include <stdnoreturn.h>

/* But did he ever return?  No he never returned,
   And his fate is still unlearned ... */
static noreturn void MTA (void);

static _Noreturn void
Charlie (void)
{
  MTA ();
}

static void
MTA (void)
{
  Charlie ();
}

int
main (int argc, char **argv)
{
  if (argc <= 0)
    MTA ();
  if (!argv[0][0])
    Charlie ();
  return 0;
}
