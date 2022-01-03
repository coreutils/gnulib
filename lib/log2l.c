/* Base 2 logarithm.
   Copyright (C) 2011-2022 Free Software Foundation, Inc.

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

#include <config.h>

/* Specification.  */
#include <math.h>

#if HAVE_SAME_LONG_DOUBLE_AS_DOUBLE

long double
log2l (long double x)
{
  return log2 (x);
}

#else

/* Best possible approximation of log(2) as a 'long double'.  */
#define LOG2 0.693147180559945309417232121458176568075L

/* Best possible approximation of 1/log(2) as a 'long double'.  */
#define LOG2_INVERSE 1.44269504088896340735992468100189213743L

/* sqrt(0.5).  */
#define SQRT_HALF 0.707106781186547524400844362104849039284L

long double
log2l (long double x)
{
  if (isnanl (x))
    return x;

  if (x <= 0.0L)
    {
      if (x == 0.0L)
        /* Return -Infinity.  */
        return - HUGE_VALL;
      else
        {
          /* Return NaN.  */
#if (defined _MSC_VER && !defined __clang__) || (defined __sgi && !defined __GNUC__)
          static long double zero;
          return zero / zero;
#else
          return 0.0L / 0.0L;
#endif
        }
    }

  /* Decompose x into
       x = 2^e * y
     where
       e is an integer,
       1/2 < y < 2.
     Then log2(x) = e + log2(y) = e + log(y)/log(2).  */
  {
    int e;
    long double y;

    y = frexpl (x, &e);
    if (y < SQRT_HALF)
      {
        y = 2.0L * y;
        e = e - 1;
      }

    return (long double) e + logl (y) * LOG2_INVERSE;
  }
}

#endif
