/* Test of rint() function.
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

/* Written by Bruno Haible <bruno@clisp.org>, 2010.  */

#include <config.h>

#include <math.h>

#include "signature.h"
SIGNATURE_CHECK (rint, double, (double));

#include "macros.h"

volatile double x;
double y;

int
main ()
{
  /* Assume round-to-nearest rounding (the default in IEEE 754).  */

  x = 2.1;
  y = rint (x);
  ASSERT (y == 2.0);

  x = -2.1;
  y = rint (x);
  ASSERT (y == -2.0);

  x = 2.7;
  y = rint (x);
  ASSERT (y == 3.0);

  x = -2.7;
  y = rint (x);
  ASSERT (y == -3.0);

  x = 2.5;
  y = rint (x);
  ASSERT (y == 2.0);

  x = 3.5;
  y = rint (x);
  ASSERT (y == 4.0);

  return 0;
}
