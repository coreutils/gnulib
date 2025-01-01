/* Multiply a 'float' by a power of 2.
   Copyright 2002-2003, 2007-2025 Free Software Foundation, Inc.

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

/* Avoid some warnings from "gcc -Wshadow".
   This file doesn't use the exp() function.  */
#undef exp
#define exp exponent

#if HAVE_SAME_LONG_DOUBLE_AS_DOUBLE

long double
ldexpl (long double x, int exp)
{
  return ldexp (x, exp);
}

#else

# define USE_LONG_DOUBLE
# include "ldexp.c"

#endif

#if 0
int
main (void)
{
  long double x;
  int y;
  for (y = 0; y < 29; y++)
    printf ("%5d %.16Lg %.16Lg\n", y, ldexpl (0.8L, y), ldexpl (0.8L, -y) * ldexpl (0.8L, y));
}
#endif
