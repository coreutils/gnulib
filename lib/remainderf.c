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

float
remainderf (float x, float y)
{
#if HAVE_REMAINDER
  return (float) remainder ((double) x, (double) y);
#else
  float q = - roundf (x / y);
  float r = fmaf (q, y, x); /* = x + q * y, computed in one step */
  /* Correct possible rounding errors in the quotient x / y.  */
  float half_y = 0.5L * y;
  if (y >= 0)
    {
      /* Expect -y/2 <= r <= y/2.  */
      if (r > half_y)
        q -= 1, r = fmaf (q, y, x);
      else if (r < - half_y)
        q += 1, r = fmaf (q, y, x);
    }
  else
    {
      /* Expect y/2 <= r <= -y/2.  */
      if (r > - half_y)
        q += 1, r = fmaf (q, y, x);
      else if (r < half_y)
        q -= 1, r = fmaf (q, y, x);
    }
  return r;
#endif
}
