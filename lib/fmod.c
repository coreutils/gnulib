/* Remainder.
   Copyright (C) 2011-2012 Free Software Foundation, Inc.

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

double
fmod (double x, double y)
{
  if (isinf (y))
    return x;
  else
    {
      double q = - trunc (x / y);
      double r = fma (q, y, x); /* = x + q * y, computed in one step */
      /* Correct possible rounding errors in the quotient x / y.  */
      if (y >= 0)
        {
          if (x >= 0)
            {
              /* Expect 0 <= r < y.  */
              if (r < 0)
                q += 1, r = fma (q, y, x);
              else if (r >= y)
                q -= 1, r = fma (q, y, x);
            }
          else
            {
              /* Expect - y < r <= 0.  */
              if (r > 0)
                q -= 1, r = fma (q, y, x);
              else if (r <= - y)
                q += 1, r = fma (q, y, x);
            }
        }
      else
        {
          if (x >= 0)
            {
              /* Expect 0 <= r < - y.  */
              if (r < 0)
                q -= 1, r = fma (q, y, x);
              else if (r >= - y)
                q += 1, r = fma (q, y, x);
            }
          else
            {
              /* Expect y < r <= 0.  */
              if (r > 0)
                q += 1, r = fma (q, y, x);
              else if (r <= y)
                q -= 1, r = fma (q, y, x);
            }
        }
      return r;
    }
}
