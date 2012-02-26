/* Test of splitting a double into fraction and mantissa.
   Copyright (C) 2012 Free Software Foundation, Inc.

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

#include <math.h>

#include "isnanf-nolibm.h"
#include "minus-zero.h"
#include "infinity.h"
#include "nan.h"
#include "macros.h"

int
main ()
{
  /* [MX] shaded specification in POSIX.  */

  /* NaN.  */
  {
    int exp = -9999;
    float mantissa;
    mantissa = frexpf (NaNf (), &exp);
    ASSERT (isnanf (mantissa));
  }

  /* Signed zero.  */
  {
    int exp = -9999;
    float mantissa;
    mantissa = frexpf (0.0f, &exp);
    ASSERT (mantissa == 0.0f);
    ASSERT (!signbit (mantissa));
    ASSERT (exp == 0);
  }
  {
    int exp = -9999;
    float mantissa;
    mantissa = frexpf (minus_zerof, &exp);
    ASSERT (mantissa == 0.0f);
    ASSERT (!!signbit (mantissa) == !!signbit (minus_zerof));
    ASSERT (exp == 0);
  }

  /* Infinity.  */
  {
    int exp = -9999;
    float mantissa;
    mantissa = frexpf (Infinityf (), &exp);
    ASSERT (mantissa == Infinityf ());
  }

  return 0;
}
