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

#include "isnanl-nolibm.h"
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
    long double mantissa;
    mantissa = frexpl (NaNl (), &exp);
    ASSERT (isnanl (mantissa));
  }

  /* Signed zero.  */
  {
    int exp = -9999;
    long double mantissa;
    mantissa = frexpl (0.0L, &exp);
    ASSERT (mantissa == 0.0L);
    ASSERT (!signbit (mantissa));
    ASSERT (exp == 0);
  }
  {
    int exp = -9999;
    long double mantissa;
    mantissa = frexpl (minus_zerol, &exp);
    ASSERT (mantissa == 0.0L);
    ASSERT (!!signbit (mantissa) == !!signbit (minus_zerol));
    ASSERT (exp == 0);
  }

  /* Infinity.  */
  {
    int exp = -9999;
    long double mantissa;
    mantissa = frexpl (Infinityl (), &exp);
    ASSERT (mantissa == Infinityl ());
  }
  {
    int exp = -9999;
    long double mantissa;
    mantissa = frexpl (- Infinityl (), &exp);
    ASSERT (mantissa == - Infinityl ());
  }

  return 0;
}
