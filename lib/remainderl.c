/* Remainder.
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

/* Specification.  */
#include <math.h>

#if HAVE_SAME_LONG_DOUBLE_AS_DOUBLE

long double
remainderl (long double x, long double y)
{
  return remainder (x, y);
}

#else

long double
remainderl (long double x, long double y)
{
  long double q = - roundl (x / y);
  long double r = fmal (q, y, x); /* = x + q * y, computed in one step */
  /* Correct possible rounding errors in the quotient x / y.  */
  long double half_y = 0.5L * y;
  if (y >= 0)
    {
      /* Expect -y/2 <= r <= y/2.  */
      if (r > half_y)
        q -= 1, r = fmal (q, y, x);
      else if (r < - half_y)
        q += 1, r = fmal (q, y, x);
    }
  else
    {
      /* Expect y/2 <= r <= -y/2.  */
      if (r > - half_y)
        q += 1, r = fmal (q, y, x);
      else if (r < half_y)
        q -= 1, r = fmal (q, y, x);
    }
  return r;
}

#endif
