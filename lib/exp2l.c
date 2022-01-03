/* Exponential base 2 function.
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
exp2l (long double x)
{
  return exp2 (x);
}

#else

# include <float.h>

/* gl_expl_table[i] = exp((i - 128) * log(2)/256).  */
extern const long double gl_expl_table[257];

/* Best possible approximation of log(2) as a 'long double'.  */
#define LOG2 0.693147180559945309417232121458176568075L

/* Best possible approximation of 1/log(2) as a 'long double'.  */
#define LOG2_INVERSE 1.44269504088896340735992468100189213743L

/* Best possible approximation of log(2)/256 as a 'long double'.  */
#define LOG2_BY_256 0.00270760617406228636491106297444600221904L

/* Best possible approximation of 256/log(2) as a 'long double'.  */
#define LOG2_BY_256_INVERSE 369.329930467574632284140718336484387181L

long double
exp2l (long double x)
{
  /* exp2(x) = exp(x*log(2)).
     If we would compute it like this, there would be rounding errors for
     integer or near-integer values of x.  To avoid these, we inline the
     algorithm for exp(), and the multiplication with log(2) cancels a
     division by log(2).  */

  if (isnanl (x))
    return x;

  if (x > (long double) LDBL_MAX_EXP)
    /* x > LDBL_MAX_EXP
       hence exp2(x) > 2^LDBL_MAX_EXP, overflows to Infinity.  */
    return HUGE_VALL;

  if (x < (long double) (LDBL_MIN_EXP - 1 - LDBL_MANT_DIG))
    /* x < (LDBL_MIN_EXP - 1 - LDBL_MANT_DIG)
       hence exp2(x) < 2^(LDBL_MIN_EXP-1-LDBL_MANT_DIG),
       underflows to zero.  */
    return 0.0L;

  /* Decompose x into
       x = n + m/256 + y/log(2)
     where
       n is an integer,
       m is an integer, -128 <= m <= 128,
       y is a number, |y| <= log(2)/512 + epsilon = 0.00135...
     Then
       exp2(x) = 2^n * exp(m * log(2)/256) * exp(y)
     The first factor is an ldexpl() call.
     The second factor is a table lookup.
     The third factor is computed
     - either as sinh(y) + cosh(y)
       where sinh(y) is computed through the power series:
         sinh(y) = y + y^3/3! + y^5/5! + ...
       and cosh(y) is computed as hypot(1, sinh(y)),
     - or as exp(2*z) = (1 + tanh(z)) / (1 - tanh(z))
       where z = y/2
       and tanh(z) is computed through its power series:
         tanh(z) = z
                   - 1/3 * z^3
                   + 2/15 * z^5
                   - 17/315 * z^7
                   + 62/2835 * z^9
                   - 1382/155925 * z^11
                   + 21844/6081075 * z^13
                   - 929569/638512875 * z^15
                   + ...
       Since |z| <= log(2)/1024 < 0.0007, the relative contribution of the
       z^13 term is < 0.0007^12 < 2^-120 <= 2^-LDBL_MANT_DIG, therefore we
       can truncate the series after the z^11 term.  */

  {
    long double nm = roundl (x * 256.0L); /* = 256 * n + m */
    long double z = (x * 256.0L - nm) * (LOG2_BY_256 * 0.5L);

/* Coefficients of the power series for tanh(z).  */
#define TANH_COEFF_1   1.0L
#define TANH_COEFF_3  -0.333333333333333333333333333333333333334L
#define TANH_COEFF_5   0.133333333333333333333333333333333333334L
#define TANH_COEFF_7  -0.053968253968253968253968253968253968254L
#define TANH_COEFF_9   0.0218694885361552028218694885361552028218L
#define TANH_COEFF_11 -0.00886323552990219656886323552990219656886L
#define TANH_COEFF_13  0.00359212803657248101692546136990581435026L
#define TANH_COEFF_15 -0.00145583438705131826824948518070211191904L

    long double z2 = z * z;
    long double tanh_z =
      (((((TANH_COEFF_11
           * z2 + TANH_COEFF_9)
          * z2 + TANH_COEFF_7)
         * z2 + TANH_COEFF_5)
        * z2 + TANH_COEFF_3)
       * z2 + TANH_COEFF_1)
      * z;

    long double exp_y = (1.0L + tanh_z) / (1.0L - tanh_z);

    int n = (int) roundl (nm * (1.0L / 256.0L));
    int m = (int) nm - 256 * n;

    return ldexpl (gl_expl_table[128 + m] * exp_y, n);
  }
}

#endif
