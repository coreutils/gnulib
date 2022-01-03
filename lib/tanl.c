/* tan (tangent) function with 'long double' argument.

   Copyright (C) 2003-2022 Free Software Foundation, Inc.

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

/* s_tanl.c -- long double version of s_tan.c.
 * Conversion to IEEE quad long double by Jakub Jelinek, jj@ultra.linux.cz.
 */

/* @(#)s_tan.c 5.1 93/09/24 */
/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice
 * is preserved.
 * ====================================================
 */

#include <config.h>

/* Specification.  */
#include <math.h>

#if HAVE_SAME_LONG_DOUBLE_AS_DOUBLE

long double
tanl (long double x)
{
  return tan (x);
}

#else

/* Code based on glibc/sysdeps/ieee754/ldbl-128/s_tanl.c
   and           glibc/sysdeps/ieee754/ldbl-128/k_tanl.c.  */

/* tanl(x)
 * Return tangent function of x.
 *
 * kernel function:
 *      __kernel_tanl           ... tangent function on [-pi/4,pi/4]
 *      __ieee754_rem_pio2l     ... argument reduction routine
 *
 * Method.
 *      Let S,C and T denote the sin, cos and tan respectively on
 *      [-PI/4, +PI/4]. Reduce the argument x to y1+y2 = x-k*pi/2
 *      in [-pi/4 , +pi/4], and let n = k mod 4.
 *      We have
 *
 *          n        sin(x)      cos(x)        tan(x)
 *     ----------------------------------------------------------
 *          0          S           C             T
 *          1          C          -S            -1/T
 *          2         -S          -C             T
 *          3         -C           S            -1/T
 *     ----------------------------------------------------------
 *
 * Special cases:
 *      Let trig be any of sin, cos, or tan.
 *      trig(+-INF)  is NaN, with signals;
 *      trig(NaN)    is that NaN;
 *
 * Accuracy:
 *      TRIG(x) returns trig(x) nearly rounded
 */

# include "trigl.h"

/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice
 * is preserved.
 * ====================================================
 */

/*
  Long double expansions contributed by
  Stephen L. Moshier <moshier@na-net.ornl.gov>
*/

/* __kernel_tanl( x, y, k )
 * kernel tan function on [-pi/4, pi/4], pi/4 ~ 0.7854
 * Input x is assumed to be bounded by ~pi/4 in magnitude.
 * Input y is the tail of x.
 * Input k indicates whether tan (if k=1) or
 * -1/tan (if k= -1) is returned.
 *
 * Algorithm
 *      1. Since tan(-x) = -tan(x), we need only to consider positive x.
 *      2. if x < 2^-57, return x with inexact if x!=0.
 *      3. tan(x) is approximated by a rational form x + x^3 / 3 + x^5 R(x^2)
 *          on [0,0.67433].
 *
 *         Note: tan(x+y) = tan(x) + tan'(x)*y
 *                        ~ tan(x) + (1+x*x)*y
 *         Therefore, for better accuracy in computing tan(x+y), let
 *              r = x^3 * R(x^2)
 *         then
 *              tan(x+y) = x + (x^3 / 3 + (x^2 *(r+y)+y))
 *
 *      4. For x in [0.67433,pi/4],  let y = pi/4 - x, then
 *              tan(x) = tan(pi/4-y) = (1-tan(y))/(1+tan(y))
 *                     = 1 - 2*(tan(y) - (tan(y)^2)/(1+tan(y)))
 */


static const long double
  pio4hi = 7.8539816339744830961566084581987569936977E-1L,
  pio4lo = 2.1679525325309452561992610065108379921906E-35L,

  /* tan x = x + x^3 / 3 + x^5 T(x^2)/U(x^2)
     0 <= x <= 0.6743316650390625
     Peak relative error 8.0e-36  */
 TH =  3.333333333333333333333333333333333333333E-1L,
 T0 = -1.813014711743583437742363284336855889393E7L,
 T1 =  1.320767960008972224312740075083259247618E6L,
 T2 = -2.626775478255838182468651821863299023956E4L,
 T3 =  1.764573356488504935415411383687150199315E2L,
 T4 = -3.333267763822178690794678978979803526092E-1L,

 U0 = -1.359761033807687578306772463253710042010E8L,
 U1 =  6.494370630656893175666729313065113194784E7L,
 U2 = -4.180787672237927475505536849168729386782E6L,
 U3 =  8.031643765106170040139966622980914621521E4L,
 U4 = -5.323131271912475695157127875560667378597E2L;
  /* 1.000000000000000000000000000000000000000E0 */


static long double
kernel_tanl (long double x, long double y, int iy)
{
  long double z, r, v, w, s, u, u1;
  int invert = 0, sign;

  sign = 1;
  if (x < 0)
    {
      x = -x;
      y = -y;
      sign = -1;
    }

  if (x < 0.000000000000000006938893903907228377647697925567626953125L) /* x < 2**-57 */
    {
      if ((int) x == 0)
        {                       /* generate inexact */
          if (iy == -1 && x == 0.0)
            return 1.0L / fabs (x);
          else
            return (iy == 1) ? x : -1.0L / x;
        }
    }
  if (x >= 0.6743316650390625) /* |x| >= 0.6743316650390625 */
    {
      invert = 1;

      z = pio4hi - x;
      w = pio4lo - y;
      x = z + w;
      y = 0.0;
    }
  z = x * x;
  r = T0 + z * (T1 + z * (T2 + z * (T3 + z * T4)));
  v = U0 + z * (U1 + z * (U2 + z * (U3 + z * (U4 + z))));
  r = r / v;

  s = z * x;
  r = y + z * (s * r + y);
  r += TH * s;
  w = x + r;
  if (invert)
    {
      v = (long double) iy;
      w = (v - 2.0 * (x - (w * w / (w + v) - r)));
      if (sign < 0)
        w = -w;
      return w;
    }
  if (iy == 1)
    return w;
  else
    {                           /* if allow error up to 2 ulp,
                                   simply return -1.0/(x+r) here */
      /*  compute -1.0/(x+r) accurately */
      u1 = (double) w;
      v = r - (u1 - x);
      z = -1.0 / w;
      u = (double) z;
      s = 1.0 + u * u1;
      return u + z * (s + u * v);
    }
}

long double
tanl (long double x)
{
  long double y[2], z = 0.0L;
  int n;

  /* tanl(NaN) is NaN */
  if (isnanl (x))
    return x;

  /* |x| ~< pi/4 */
  if (x >= -0.7853981633974483096156608458198757210492 &&
      x <= 0.7853981633974483096156608458198757210492)
    return kernel_tanl (x, z, 1);

  /* tanl(Inf) is NaN, tanl(0) is 0 */
  else if (x + x == x)
    return x - x;               /* NaN */

  /* argument reduction needed */
  else
    {
      n = ieee754_rem_pio2l (x, y);
      /* 1 -- n even, -1 -- n odd */
      return kernel_tanl (y[0], y[1], 1 - ((n & 1) << 1));
    }
}

#endif

#if 0
int
main (void)
{
  printf ("%.16Lg\n", tanl (0.7853981633974483096156608458198757210492));
  printf ("%.16Lg\n", tanl (-0.7853981633974483096156608458198757210492));
  printf ("%.16Lg\n", tanl (0.7853981633974483096156608458198757210492 *3));
  printf ("%.16Lg\n", tanl (-0.7853981633974483096156608458198757210492 *31));
  printf ("%.16Lg\n", tanl (0.7853981633974483096156608458198757210492 / 2));
  printf ("%.16Lg\n", tanl (0.7853981633974483096156608458198757210492 * 3/2));
  printf ("%.16Lg\n", tanl (0.7853981633974483096156608458198757210492 * 5/2));
}
#endif
