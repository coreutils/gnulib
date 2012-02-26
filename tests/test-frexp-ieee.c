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

#include "isnand-nolibm.h"
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
    double mantissa;
    mantissa = frexp (NaNd (), &exp);
    ASSERT (isnand (mantissa));
  }

  /* Signed zero.  */
  {
    int exp = -9999;
    double mantissa;
    mantissa = frexp (0.0, &exp);
    ASSERT (mantissa == 0.0);
    ASSERT (!signbit (mantissa));
    ASSERT (exp == 0);
  }
  {
    int exp = -9999;
    double mantissa;
    mantissa = frexp (minus_zerod, &exp);
    ASSERT (mantissa == 0.0);
    ASSERT (!!signbit (mantissa) == !!signbit (minus_zerod));
    ASSERT (exp == 0);
  }

  /* Infinity.  */
  {
    int exp = -9999;
    double mantissa;
    mantissa = frexp (Infinityd (), &exp);
    ASSERT (mantissa == Infinityd ());
  }
  {
    int exp = -9999;
    double mantissa;
    mantissa = frexp (- Infinityd (), &exp);
    ASSERT (mantissa == - Infinityd ());
  }

  return 0;
}
