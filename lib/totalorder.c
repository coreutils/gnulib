/* Total order for 'double'
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
totalorder (double const *x, double const *y)
{
  /* Although the following is not strictly portable, and won't work
     on some obsolete platforms (e.g., PA-RISC, MIPS before alignment
     to IEEE 754-2008), it should be good enough nowadays.  */
  int xs = signbit (*x);
  int ys = signbit (*y);
  if (!xs != !ys)
    return xs;
  int xn = isnand (*x);
  int yn = isnand (*y);
  if (!xn != !yn)
    return !xn == !xs;
  if (!xn)
    return *x <= *y;

  unsigned long long extended_sign = -!!xs;
  union { unsigned long long i; double f; } volatile xu = {0}, yu = {0};
  xu.f = *x;
  yu.f = *y;
  return (xu.i ^ extended_sign) <= (yu.i ^ extended_sign);
}
