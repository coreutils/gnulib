/* Floating-point exponent.
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

#include <limits.h>

#ifdef USE_LONG_DOUBLE
# define ILOGB ilogbl
# define DOUBLE long double
# define L_(literal) literal##L
# define FREXP frexpl
# define ISNAN isnanl
#elif ! defined USE_FLOAT
# define ILOGB ilogb
# define DOUBLE double
# define L_(literal) literal
# define FREXP frexp
# define ISNAN isnand
#else /* defined USE_FLOAT */
# define ILOGB ilogbf
# define DOUBLE float
# define L_(literal) literal##f
# define FREXP frexpf
# define ISNAN isnanf
#endif

int
ILOGB (DOUBLE x)
{
  if (isfinite (x))
    {
      if (x == L_(0.0))
        return FP_ILOGB0;
      else
        {
          int e;

          (void) FREXP (x, &e);
          return e - 1;
        }
    }
  else
    {
      if (ISNAN (x))
        return FP_ILOGBNAN;
      else
        return INT_MAX;
    }
}
