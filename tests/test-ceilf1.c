/* Test of rounding towards positive infinity.
   Copyright (C) 2007-2010 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2007.  */

#include <config.h>

#include <math.h>

#include "signature.h"
SIGNATURE_CHECK (ceilf, float, (float));

#include "isnanf-nolibm.h"
#include "nan.h"
#include "macros.h"

/* HP cc on HP-UX 10.20 has a bug with the constant expression -0.0f.
   So we use -zero instead.  */
float zero = 0.0f;

int
main ()
{
  /* Zero.  */
  ASSERT (ceilf (0.0f) == 0.0f);
  ASSERT (ceilf (-zero) == 0.0f);
  /* Positive numbers.  */
  ASSERT (ceilf (0.3f) == 1.0f);
  ASSERT (ceilf (0.7f) == 1.0f);
  ASSERT (ceilf (1.0f) == 1.0f);
  ASSERT (ceilf (1.001f) == 2.0f);
  ASSERT (ceilf (1.5f) == 2.0f);
  ASSERT (ceilf (1.999f) == 2.0f);
  ASSERT (ceilf (2.0f) == 2.0f);
  ASSERT (ceilf (65535.99f) == 65536.0f);
  ASSERT (ceilf (65536.0f) == 65536.0f);
  ASSERT (ceilf (2.341e31f) == 2.341e31f);
  /* Negative numbers.  */
  ASSERT (ceilf (-0.3f) == 0.0f);
  ASSERT (ceilf (-0.7f) == 0.0f);
  ASSERT (ceilf (-1.0f) == -1.0f);
  ASSERT (ceilf (-1.5f) == -1.0f);
  ASSERT (ceilf (-1.999f) == -1.0f);
  ASSERT (ceilf (-2.0f) == -2.0f);
  ASSERT (ceilf (-65535.99f) == -65535.0f);
  ASSERT (ceilf (-65536.0f) == -65536.0f);
  ASSERT (ceilf (-2.341e31f) == -2.341e31f);
  /* Infinite numbers.  */
  ASSERT (ceilf (1.0f / 0.0f) == 1.0f / 0.0f);
  ASSERT (ceilf (-1.0f / 0.0f) == -1.0f / 0.0f);
  /* NaNs.  */
  ASSERT (isnanf (ceilf (NaNf ())));

  return 0;
}
