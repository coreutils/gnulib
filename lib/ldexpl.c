/* Emulation for ldexpl.
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

long double
ldexpl(long double x, int exp)
{
  long double factor;
  int bit;

  /* Check for zero, nan and infinity. */
  if (x != x || x + x == x )
    return x;

  if (exp < 0)
    {
      exp = -exp;
      factor = 0.5L;
    }
  else
    factor = 2.0L;

  for (bit = 1; bit <= exp; bit <<= 1, factor *= factor)
    if (exp & bit)
      x *= factor;

  return x;
}

#if 0
int
main()
{
  long double x;
  int y;
  for (y = 0; y < 29; y++)
    printf ("%5d %.16Lg %.16Lg\n", y, ldexpl(0.8L, y), ldexpl(0.8L, -y) * ldexpl(0.8L, y));
}
#endif
