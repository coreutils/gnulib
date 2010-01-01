/* Test of multiplying a 'long double' by a power of 2.
   Copyright (C) 2007-2010 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2007.  */

#include <config.h>

#include <math.h>

#include "signature.h"
SIGNATURE_CHECK (ldexpl, long double, (long double, int));

#include <float.h>

#include "fpucw.h"
#include "isnanl-nolibm.h"
#include "nan.h"
#include "macros.h"

/* On HP-UX 10.20, negating 0.0L does not yield -0.0L.
   So we use minus_zero instead.
   IRIX cc can't put -0.0L into .data, but can compute at runtime.
   Note that the expression -LDBL_MIN * LDBL_MIN does not work on other
   platforms, such as when cross-compiling to PowerPC on MacOS X 10.5.  */
#if defined __hpux || defined __sgi
static long double
compute_minus_zero (void)
{
  return -LDBL_MIN * LDBL_MIN;
}
# define minus_zero compute_minus_zero ()
#else
long double minus_zero = -0.0L;
#endif

int
main ()
{
  int i;
  long double x;
  long double y;
  DECL_LONG_DOUBLE_ROUNDING

  BEGIN_LONG_DOUBLE_ROUNDING ();

  { /* NaN.  */
    x = NaNl ();
    y = ldexpl (x, 0); ASSERT (isnanl (y));
    y = ldexpl (x, 5); ASSERT (isnanl (y));
    y = ldexpl (x, -5); ASSERT (isnanl (y));
  }

  { /* Positive infinity.  */
    x = 1.0L / 0.0L;
    y = ldexpl (x, 0); ASSERT (y == x);
    y = ldexpl (x, 5); ASSERT (y == x);
    y = ldexpl (x, -5); ASSERT (y == x);
  }

  { /* Negative infinity.  */
    x = -1.0L / 0.0L;
    y = ldexpl (x, 0); ASSERT (y == x);
    y = ldexpl (x, 5); ASSERT (y == x);
    y = ldexpl (x, -5); ASSERT (y == x);
  }

  { /* Positive zero.  */
    x = 0.0L;
    y = ldexpl (x, 0); ASSERT (y == x); ASSERT (!signbit (x));
    y = ldexpl (x, 5); ASSERT (y == x); ASSERT (!signbit (x));
    y = ldexpl (x, -5); ASSERT (y == x); ASSERT (!signbit (x));
  }

  { /* Negative zero.  */
    x = minus_zero;
    y = ldexpl (x, 0); ASSERT (y == x); ASSERT (signbit (x));
    y = ldexpl (x, 5); ASSERT (y == x); ASSERT (signbit (x));
    y = ldexpl (x, -5); ASSERT (y == x); ASSERT (signbit (x));
  }

  { /* Positive finite number.  */
    x = 1.73205L;
    y = ldexpl (x, 0); ASSERT (y == x);
    y = ldexpl (x, 5); ASSERT (y == x * 32.0L);
    y = ldexpl (x, -5); ASSERT (y == x * 0.03125L);
  }

  { /* Negative finite number.  */
    x = -20.085536923187667742L;
    y = ldexpl (x, 0); ASSERT (y == x);
    y = ldexpl (x, 5); ASSERT (y == x * 32.0L);
    y = ldexpl (x, -5); ASSERT (y == x * 0.03125L);
  }

  for (i = 1, x = 1.73205L; i <= LDBL_MAX_EXP; i++, x *= 2.0L)
    {
      y = ldexpl (x, 0); ASSERT (y == x);
      y = ldexpl (x, 5); ASSERT (y == x * 32.0L);
      y = ldexpl (x, -5); ASSERT (y == x * 0.03125L);
    }
  for (i = 1, x = 1.73205L; i >= LDBL_MIN_EXP; i--, x *= 0.5L)
    {
      y = ldexpl (x, 0); ASSERT (y == x);
      y = ldexpl (x, 5); ASSERT (y == x * 32.0L);
      if (i - 5 >= LDBL_MIN_EXP)
        {
          y = ldexpl (x, -5); ASSERT (y == x * 0.03125L);
        }
    }
  for (; i >= LDBL_MIN_EXP - 100 && x > 0.0L; i--, x *= 0.5L)
    {
      y = ldexpl (x, 0); ASSERT (y == x);
      y = ldexpl (x, 5); ASSERT (y == x * 32.0L);
    }

  return 0;
}
