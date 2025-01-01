/* Total order for 'long double'
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

#if HAVE_SAME_LONG_DOUBLE_AS_DOUBLE

int
totalorderl (long double const *x, long double const *y)
{
  return totalorder ((double const *) x, (double const *) y);
}

#else

# include <float.h>

# ifndef LDBL_SIGNBIT_WORD
#  define LDBL_SIGNBIT_WORD (-1)
# endif

int
totalorderl (long double const *x, long double const *y)
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
  unsigned long long extended_sign_hi = extended_sign;
# if defined __hppa || (defined __mips__ && !MIPS_NAN2008_LONG_DOUBLE) || defined __sh__
  /* Invert the most significant bit of the mantissa field.  Cf. snan.h.  */
  extended_sign_hi ^=
    (1ULL << (LDBL_MANT_DIG == 106
              ? 51                          /* double-double representation */
              : (LDBL_MANT_DIG - 2) - 64)); /* quad precision representation */
# endif
  union u { unsigned long long i[2]; long double f; } volatile xu, yu;
  /* Although it is tempting to initialize with {0}, Solaris cc (Sun C 5.8)
     on x86_64 miscompiles {0}: it initializes only the lower 80 bits,
     not the entire 128 bits.  */
  xu.i[0] = 0; xu.i[1] = 0;
  yu.i[0] = 0; yu.i[1] = 0;
  xu.f = *x;
  yu.f = *y;

  /* Set BIGENDIAN to true if and only if the most significant bits of
     xu.f's fraction are in xu.i[0].  Use the sign bit's location to
     infer BIGENDIAN.  */
  bool bigendian;
  if (LDBL_SIGNBIT_WORD < 0)
    {
      union u volatile zu;
      zu.i[0] = 0; zu.i[1] = 0;
      zu.f = -zu.f;
      bigendian = !!zu.i[0];
    }
  else
    bigendian = LDBL_SIGNBIT_WORD < sizeof xu.i[0] / sizeof (unsigned);

  unsigned long long
    xhi = xu.i[!bigendian] ^ extended_sign_hi,
    yhi = yu.i[!bigendian] ^ extended_sign_hi,
    xlo = xu.i[ bigendian] ^ extended_sign,
    ylo = yu.i[ bigendian] ^ extended_sign;
  return (xhi < yhi) | ((xhi == yhi) & (xlo <= ylo));
}

#endif
