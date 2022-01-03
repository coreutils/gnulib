/* Get signed integer and fractional parts of a floating-point number.
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

#include <config.h>

/* Specification.  */
#include <math.h>

#if HAVE_SAME_LONG_DOUBLE_AS_DOUBLE

long double
modfl (long double x, long double *iptr)
{
  double integer_part;
  double fractional_part = modf (x, &integer_part);
  *iptr = integer_part;
  return fractional_part;
}

#else

long double
modfl (long double x, long double *iptr)
{
  if (isfinite (x))
    {
      long double integer_part = truncl (x);
      *iptr = integer_part;
      return x - integer_part;
    }
  else
    {
      if (isinf (x))
        {
          *iptr = x;
          return 1.0L / x;
        }
      else /* isnanl (x) */
        {
          *iptr = x;
          return x;
        }
    }
}

#endif
