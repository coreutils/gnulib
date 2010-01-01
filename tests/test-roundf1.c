/* Test of rounding to nearest, breaking ties away from zero.
   Copyright (C) 2007, 2008, 2009, 2010 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

/* Written by Ben Pfaff <blp@gnu.org>, 2007.
   Based heavily on Bruno Haible's test-truncf.c. */

#include <config.h>

#include <math.h>

#include "signature.h"
SIGNATURE_CHECK (roundf, float, (float));

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
  ASSERT (roundf (0.0f) == 0.0f);
  ASSERT (roundf (-zero) == 0.0f);
  /* Positive numbers.  */
  ASSERT (roundf (0.3f) == 0.0f);
  ASSERT (roundf (0.5f) == 1.0f);
  ASSERT (roundf (0.7f) == 1.0f);
  ASSERT (roundf (1.0f) == 1.0f);
  ASSERT (roundf (1.5f) == 2.0f);
  ASSERT (roundf (2.5f) == 3.0f);
  ASSERT (roundf (1.999f) == 2.0f);
  ASSERT (roundf (2.0f) == 2.0f);
  ASSERT (roundf (65535.99f) == 65536.0f);
  ASSERT (roundf (65536.0f) == 65536.0f);
  ASSERT (roundf (65536.01f) == 65536.0f);
  ASSERT (roundf (2.341e31f) == 2.341e31f);
  /* Negative numbers.  */
  ASSERT (roundf (-0.3f) == 0.0f);
  ASSERT (roundf (-0.5f) == -1.0f);
  ASSERT (roundf (-0.7f) == -1.0f);
  ASSERT (roundf (-1.0f) == -1.0f);
  ASSERT (roundf (-1.5f) == -2.0f);
  ASSERT (roundf (-2.5f) == -3.0f);
  ASSERT (roundf (-1.999f) == -2.0f);
  ASSERT (roundf (-2.0f) == -2.0f);
  ASSERT (roundf (-65535.99f) == -65536.0f);
  ASSERT (roundf (-65536.0f) == -65536.0f);
  ASSERT (roundf (-65536.01f) == -65536.0f);
  ASSERT (roundf (-2.341e31f) == -2.341e31f);
  /* Infinite numbers.  */
  ASSERT (roundf (1.0 / 0.0f) == 1.0 / 0.0f);
  ASSERT (roundf (-1.0 / 0.0f) == -1.0 / 0.0f);
  /* NaNs.  */
  ASSERT (isnanf (roundf (NaNf ())));

  return 0;
}
