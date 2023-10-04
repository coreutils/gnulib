/* Total order for 'long double'
   Copyright 2023 Free Software Foundation, Inc.

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

/* Written by Paul Eggert.  */

#include <config.h>

#include <math.h>

int
totalorderl (long double const *x, long double const *y)
{
  /* Although the following is not strictly portable, and won't work
     on some obsolete platforms (e.g., PA-RISC, MIPS before alignment
     to IEEE 754-2008), it should be good enough nowadays.  */

  /* If the sign bits of *X and *Y differ, the one with sign bit == 1
     is "smaller" than the one with sign bit == 0.  */
  int xs = signbit (*x);
  int ys = signbit (*y);
  if (!xs != !ys)
    return xs;

  /* If one of *X, *Y is a NaN and the other isn't, the answer is easy
     as well: the negative NaN is "smaller", the positive NaN is "greater"
     than the other argument.  */
  int xn = isnanl (*x);
  int yn = isnanl (*y);
  if (!xn != !yn)
    return !xn == !xs;
  /* If none of *X, *Y is a NaN, the '<=' operator does the job, including
     for -Infinity and +Infinity.  */
  if (!xn)
    return *x <= *y;

  /* At this point, *X and *Y are NaNs with the same sign bit.  */

  unsigned long long extended_sign = -!!xs;

  if (sizeof (long double) <= sizeof (unsigned long long))
    {
      union { unsigned long long i; long double f; } volatile
        xu = {0}, yu = {0};
      xu.f = *x;
      yu.f = *y;
      return (xu.i ^ extended_sign) <= (yu.i ^ extended_sign);
    }

  union { unsigned long long i[2]; long double f; } volatile xu, yu, zu;
  /* It would be tempting to initialize xu, yu, zu with {0}.  But
     Solaris cc (Sun C 5.8) on x86_64 miscompiles it: It initializes
     only the lower 80 bits, not the entire 128 bits, of each of the
     three variables.  */
  xu.i[0] = 0; xu.i[1] = 0;
  yu.i[0] = 0; yu.i[1] = 0;
  zu.i[0] = 0; zu.i[1] = 0;

  xu.f = *x;
  yu.f = *y;

  /* Determine in which of the two 'unsigned long long' words the sign bit
     is located.  */
  bool bigendian;
#if defined LDBL_SIGNBIT_WORD
  /* We have already determined the sign bit location at configure time.  */
  bigendian = (LDBL_SIGNBIT_WORD < 2);
#else
  zu.f = -zu.f;
  bigendian = !!zu.i[0];
#endif

  unsigned long long
    xhi = xu.i[!bigendian] ^ extended_sign,
    yhi = yu.i[!bigendian] ^ extended_sign,
    xlo = xu.i[ bigendian] ^ extended_sign,
    ylo = yu.i[ bigendian] ^ extended_sign;
  return (xhi < yhi) | ((xhi == yhi) & (xlo <= ylo));
}
