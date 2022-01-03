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

#if HAVE_SAME_LONG_DOUBLE_AS_DOUBLE

long double
hypotl (long double x, long double y)
{
  return hypot (x, y);
}

#else

long double
hypotl (long double x, long double y)
{
  if (isfinite (x) && isfinite (y))
    {
      /* Determine absolute values.  */
      x = fabsl (x);
      y = fabsl (y);

      {
        /* Find the bigger and the smaller one.  */
        long double a;
        long double b;

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
          long double an;
          long double bn;

          /* Write a = an * 2^e, b = bn * 2^e with 0 <= bn <= an < 1.  */
          an = frexpl (a, &e);
          bn = ldexpl (b, - e);

          {
            long double cn;

            /* Through the normalization, no unneeded overflow or underflow
               will occur here.  */
            cn = sqrtl (an * an + bn * bn);
            return ldexpl (cn, e);
          }
        }
      }
    }
  else
    {
      if (isinf (x) || isinf (y))
        /* x or y is infinite.  Return +Infinity.  */
        return HUGE_VALL;
      else
        /* x or y is NaN.  Return NaN.  */
        return x + y;
    }
}

#endif
