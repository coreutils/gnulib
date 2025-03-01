/* Test of atan2f() function.
   Copyright (C) 2010-2025 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Written by Bruno Haible <bruno@clisp.org>, 2010-2011.  */

#include <config.h>

#include <math.h>

#include "signature.h"
SIGNATURE_CHECK (atan2f, float, (float, float));

#include "macros.h"

volatile float x;
volatile float y;
float z;

int
main ()
{
  /* A particular value in the first quadrant.  */
  x = 0.6f;
  y = 0.8f;
  z = atan2f (y, x);
  ASSERT (z >= 0.92729515f && z <= 0.9272953f);

  /* A particular value in the second quadrant.  */
  x = -0.6f;
  y = 0.8f;
  z = atan2f (y, x);
  ASSERT (z >= 2.2142974f && z <= 2.2142975f);

  /* A particular value in the third quadrant.  */
  x = -0.6f;
  y = -0.8f;
  z = atan2f (y, x);
  ASSERT (z >= -2.2142975f && z <= -2.2142974f);

  /* A particular value in the fourth quadrant.  */
  x = 0.6f;
  y = -0.8f;
  z = atan2f (y, x);
  ASSERT (z >= -0.9272953f && z <= -0.92729515f);

  return test_exit_status;
}
