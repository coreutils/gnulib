/* Total order for 'float'
   Copyright 2023-2024 Free Software Foundation, Inc.

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

int
totalorderf (float const *x, float const *y)
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
  int xn = isnanf (*x);
  int yn = isnanf (*y);
  if (!xn != !yn)
    return !xn == !xs;
  /* If none of *X, *Y is a NaN, the '<=' operator does the job, including
     for -Infinity and +Infinity.  */
  if (!xn)
    return *x <= *y;

  /* At this point, *X and *Y are NaNs with the same sign bit.  */

  unsigned int extended_sign = -!!xs;
#if defined __hppa || defined __mips__ || defined __sh__
  /* Invert the most significant bit of the mantissa field.  Cf. snan.h.  */
  extended_sign ^= (1U << 22);
#endif
  union { unsigned int i; float f; } volatile xu = {0}, yu = {0};
  xu.f = *x;
  yu.f = *y;
  return (xu.i ^ extended_sign) <= (yu.i ^ extended_sign);
}
