/* Emulation for expl.
   Contributed by Paolo Bonzini

   Copyright 2002, 2003 Free Software Foundation, Inc.

   This file is part of gnulib.

   gnulib is free software; you can redistribute it and/or modify it
   under the terms of the GNU Lesser General Public License as published
   by the Free Software Foundation; either version 2.1, or (at your option)
   any later version.

   gnulib is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
   License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with gnulib; see the file COPYING.LIB.  If not, write to the Free
   Software Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307,
   USA.
 */

#include <float.h>
#include <math.h>

#include "mathl.h"

static const long double C[] = {
/* Chebyshev polynom coeficients for (exp(x)-1)/x */
#define P1 C[0]
#define P2 C[1]
#define P3 C[2]
#define P4 C[3]
#define P5 C[4]
#define P6 C[5]
 0.5L,
 1.66666666666666666666666666666666683E-01L,
 4.16666666666666666666654902320001674E-02L,
 8.33333333333333333333314659767198461E-03L,
 1.38888888889899438565058018857254025E-03L,
 1.98412698413981650382436541785404286E-04L,

/* Smallest integer x for which e^x overflows.  */
#define himark C[6]
 11356.523406294143949491931077970765L,

/* Largest integer x for which e^x underflows.  */
#define lomark C[7]
-11433.4627433362978788372438434526231L,

/* very small number */
#define TINY C[8]
 1.0e-4900L,

/* 2^16383 */
#define TWO16383 C[9]
 5.94865747678615882542879663314003565E+4931L};

long double
expl (long double x)
{
  /* Check for usual case.  */
  if (x < himark && x > lomark)
    {
      int exponent;
      long double t, x22;
      int k = 1;
      long double result = 1.0;

      /* Compute an integer power of e with a granularity of 0.125.  */
      exponent = (int) floorl (x * 8.0L);
      x -= exponent / 8.0L;

      if (x > 0.0625)
	{
	  exponent++;
	  x -= 0.125L;
	}

      if (exponent < 0)
        {
          t = 0.8824969025845954028648921432290507362220L; /* e^-0.25 */
	  exponent = -exponent;
	}
      else
        t = 1.1331484530668263168290072278117938725655L; /* e^0.25 */

      while (exponent)
        {
          if (exponent & k)
            {
              result *= t;
              exponent ^= k;
            }
          t *= t;
          k <<= 1;
        }

      /* Approximate (e^x - 1)/x, using a seventh-degree polynomial,
	 with maximum error in [-2^-16-2^-53,2^-16+2^-53]
	 less than 4.8e-39.  */
      x22 = x + x*x*(P1+x*(P2+x*(P3+x*(P4+x*(P5+x*P6)))));

      return result + result * x22;
    }
  /* Exceptional cases:  */
  else if (x < himark)
    {
      if (x + x == x)
	/* e^-inf == 0, with no error.  */
	return 0;
      else
	/* Underflow */
	return TINY * TINY;
    }
  else
    /* Return x, if x is a NaN or Inf; or overflow, otherwise.  */
    return TWO16383*x;
}

#if 0
int
main ()
{
  printf ("%.16Lg\n", expl(1.0L));
  printf ("%.16Lg\n", expl(-1.0L));
  printf ("%.16Lg\n", expl(2.0L));
  printf ("%.16Lg\n", expl(4.0L));
  printf ("%.16Lg\n", expl(-2.0L));
  printf ("%.16Lg\n", expl(-4.0L));
  printf ("%.16Lg\n", expl(0.0625L));
  printf ("%.16Lg\n", expl(0.3L));
  printf ("%.16Lg\n", expl(0.6L));
}
#endif
