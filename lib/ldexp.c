/* Multiply a 'float' by a power of 2.
   Copyright 2002-2003, 2007-2023 Free Software Foundation, Inc.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of the
   License, or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Written by Paolo Bonzini and Bruno Haible.  */

#include <config.h>

/* Specification.  */
#include <math.h>

# include <float.h>
#ifdef USE_LONG_DOUBLE
# include "fpucw.h"
#endif

/* Avoid some warnings from "gcc -Wshadow".
   This file doesn't use the exp() function.  */
#undef exp
#define exp exponent

#ifdef USE_LONG_DOUBLE
# define FUNC ldexpl
# define DOUBLE long double
# define ISNAN isnanl
# define DECL_ROUNDING DECL_LONG_DOUBLE_ROUNDING
# define BEGIN_ROUNDING() BEGIN_LONG_DOUBLE_ROUNDING ()
# define END_ROUNDING() END_LONG_DOUBLE_ROUNDING ()
# define L_(literal) literal##L
#else
# define FUNC ldexp
# define DOUBLE double
# define ISNAN isnand
# define DECL_ROUNDING
# define BEGIN_ROUNDING()
# define END_ROUNDING()
# define L_(literal) literal
#endif

DOUBLE
FUNC (DOUBLE x, int exp)
{
  DECL_ROUNDING

  BEGIN_ROUNDING ();

  /* Check for zero, nan and infinity. */
  if (!(ISNAN (x) || x + x == x))
    {
      unsigned int uexp;
      DOUBLE factor;

      if (exp < 0)
        {
          /* Avoid signed integer overflow when exp == INT_MIN.  */
          uexp = (unsigned int) (-1 - exp) + 1;
          factor = L_(0.5);
        }
      else
        {
          uexp = exp;
          factor = L_(2.0);
        }

      if (uexp > 0)
        {
          unsigned int bit;

          for (bit = 1;;)
            {
              /* Invariant: Here bit = 2^i, factor = 2^-2^i or = 2^2^i,
                 and bit <= uexp.  */
              if (uexp & bit)
                x *= factor;
              bit <<= 1;
              if (bit > uexp)
                break;
              factor = factor * factor;
            }
        }
    }

  END_ROUNDING ();

  return x;
}
