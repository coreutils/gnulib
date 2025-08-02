/* Total order for 'double'
   Copyright 2023-2025 Free Software Foundation, Inc.

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

/* Specification.  */
#include <math.h>

#include <string.h>

int
totalorder (double const *x, double const *y)
{
  /* If the sign bits of *X and *Y differ, the one with non-zero sign bit
     is "smaller" than the one with sign bit == 0.  */
  int xs = signbit (*x);
  int ys = signbit (*y);
  if (!xs != !ys)
    return xs;

  /* If one of *X, *Y is a NaN and the other isn't, the answer is easy
     as well: the negative NaN is "smaller", the positive NaN is "greater"
     than the other argument.  */
  int xn = isnand (*x);
  int yn = isnand (*y);
  if (!xn != !yn)
    return !xn == !xs;
  /* If none of *X, *Y is a NaN, the '<=' operator does the job, including
     for -Infinity and +Infinity.  */
  if (!xn)
    return *x <= *y;

  /* At this point, *X and *Y are NaNs with the same sign bit.  */

  unsigned long long extended_sign = -!!xs;
#if defined __hppa || (defined __mips__ && !MIPS_NAN2008_DOUBLE) || defined __sh__
  /* Invert the most significant bit of the mantissa field.  Cf. snan.h.  */
  extended_sign ^= (1ULL << 51);
#endif
  union { unsigned long long i; double f; } xu = {0}, yu = {0};
#if 0
  xu.f = *x;
  yu.f = *y;
#else
# if defined __GNUC__ || defined __clang__
  /* Prevent gcc and clang from reusing the values of *x and *y (fetched above)
     in optimized inlined memcpy expansions.
     Seen with gcc <https://gcc.gnu.org/PR114659>
     and with clang 16.0.6 on OpenBSD 7.5.  */
  __asm__ __volatile__ ("" : : : "memory");
# endif
  /* On 32-bit x86 processors, as well as on x86_64 processors with
     CC="gcc -mfpmath=387", the evaluation of *x and *y above is done through
     an 'fldl' instruction, which converts a signalling NaN to a quiet NaN. See
     <https://lists.gnu.org/archive/html/bug-gnulib/2023-10/msg00060.html>
     for details.  Use memcpy to avoid this.  */
  memcpy (&xu.f, x, sizeof (double));
  memcpy (&yu.f, y, sizeof (double));
#endif
  return (xu.i ^ extended_sign) <= (yu.i ^ extended_sign);
}
