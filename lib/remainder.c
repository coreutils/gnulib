/* Remainder.
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

#if ! (defined USE_LONG_DOUBLE || defined USE_FLOAT)
# include <config.h>
#endif

/* Specification.  */
#include <math.h>

#ifdef USE_LONG_DOUBLE
# define REMAINDER remainderl
# define DOUBLE long double
# define L_(literal) literal##L
# define FABS fabsl
# define FMOD fmodl
# define ISNAN isnanl
#elif ! defined USE_FLOAT
# define REMAINDER remainder
# define DOUBLE double
# define L_(literal) literal
# define FABS fabs
# define FMOD fmod
# define ISNAN isnand
#else /* defined USE_FLOAT */
# define REMAINDER remainderf
# define DOUBLE float
# define L_(literal) literal##f
# define FABS fabsf
# define FMOD fmodf
# define ISNAN isnanf
#endif

#undef NAN
#if defined _MSC_VER
static DOUBLE zero;
# define NAN (zero / zero)
#else
# define NAN (L_(0.0) / L_(0.0))
#endif

DOUBLE
REMAINDER (DOUBLE x, DOUBLE y)
{
  if (isfinite (x) && isfinite (y) && y != L_(0.0))
    {
      if (x == L_(0.0))
        /* Return x, regardless of the sign of y.  */
        return x;

      {
        int negate = ((!signbit (x)) ^ (!signbit (y)));
        DOUBLE r;

        /* Take the absolute value of x and y.  */
        x = FABS (x);
        y = FABS (y);

        /* Trivial case that requires no computation.  */
        if (x <= L_(0.5) * y)
          return (negate ? - x : x);

        /* With a fixed y, the function x -> remainder(x,y) has a period 2*y.
           Therefore we can reduce the argument x modulo 2*y.  And it's no
           problem if 2*y overflows, since fmod(x,Inf) = x.  */
        x = FMOD (x, L_(2.0) * y);

        /* Consider the 3 cases:
             0 <= x <= 0.5 * y
             0.5 * y < x < 1.5 * y
             1.5 * y <= x <= 2.0 * y  */
        if (x <= L_(0.5) * y)
          r = x;
        else
          {
            r = x - y;
            if (r > L_(0.5) * y)
              r = x - L_(2.0) * y;
          }
        return (negate ? - r : r);
      }
    }
  else
    {
      if (ISNAN (x) || ISNAN (y))
        return x + y; /* NaN */
      else if (isinf (y))
        return x;
      else
        /* x infinite or y zero */
        return NAN;
    }
}
