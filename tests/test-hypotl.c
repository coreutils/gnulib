/* Test of hypotl() function.
   Copyright (C) 2010-2012 Free Software Foundation, Inc.

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
SIGNATURE_CHECK (hypotl, long double, (long double, long double));

#include <float.h>

#include "macros.h"

volatile long double x;
volatile long double y;
long double z;

int
main ()
{
  /* A particular value.  */
  x = 0.4L;
  y = 0.6L;
  z = hypotl (x, y);
  ASSERT (z >= 0.7211102550L && z <= 0.7211102551L);

  /* Overflow.  */
  x = LDBL_MAX;
  y = LDBL_MAX * 0.5L;
  z = hypotl (x, y);
  ASSERT (z == HUGE_VALL);

  /* No underflow.  */
  x = LDBL_MIN;
  y = 0.0L;
  z = hypotl (x, y);
  ASSERT (z == LDBL_MIN);

  /* No underflow.  */
  x = LDBL_MIN * 2.0L;
  y = LDBL_MIN * 3.0L;
  z = hypotl (x, y);
  ASSERT (z >= LDBL_MIN * 2.0L && z <= LDBL_MIN * 4.0L);

  return 0;
}
