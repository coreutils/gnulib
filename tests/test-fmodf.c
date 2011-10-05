/* Test of fmodf() function.
   Copyright (C) 2010-2011 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2010-2011.  */

#include <config.h>

#include <math.h>

#include "signature.h"
SIGNATURE_CHECK (fmodf, float, (float, float));

#include "macros.h"

volatile float x;
volatile float y;
float z;

int
main ()
{
  /* A particular value.  */
  x = 9.245907f;
  y = 3.141593f;
  z = fmodf (x, y);
  ASSERT (z >= 2.962720f && z <= 2.962722f);

  return 0;
}
