/* Hypotenuse of a right-angled triangle.
   Copyright (C) 2012-2022 Free Software Foundation, Inc.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Written by Bruno Haible <bruno@clisp.org>, 2012.  */

#include <config.h>

/* Specification.  */
#include <math.h>

double
hypot (double x, double y)
{
  if (isfinite (x) && isfinite (y))
    {
      /* Determine absolute values.  */
      x = fabs (x);
      y = fabs (y);

      {
        /* Find the bigger and the smaller one.  */
        double a;
        double b;

        if (x >= y)
          {
            a = x;
            b = y;
          }
        else
          {
            a = y;
            b = x;
          }
        /* Now 0 <= b <= a.  */

        {
          int e;
          double an;
          double bn;

          /* Write a = an * 2^e, b = bn * 2^e with 0 <= bn <= an < 1.  */
          an = frexp (a, &e);
          bn = ldexp (b, - e);

          {
            double cn;

            /* Through the normalization, no unneeded overflow or underflow
               will occur here.  */
            cn = sqrt (an * an + bn * bn);
            return ldexp (cn, e);
          }
        }
      }
    }
  else
    {
      if (isinf (x) || isinf (y))
        /* x or y is infinite.  Return +Infinity.  */
        return HUGE_VAL;
      else
        /* x or y is NaN.  Return NaN.  */
        return x + y;
    }
}
