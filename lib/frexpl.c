/* Emulation for frexpl.
   Contributed by Paolo Bonzini

   Copyright 2002, 2003 Free Software Foundation, Inc.

   This file is part of gnulib.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#include <float.h>
#include <math.h>

#include "mathl.h"

/* Binary search.  Quite inefficient but portable. */
long double
frexpl(long double x, int *exp)
{
  long double exponents[20], *next;
  int exponent, bit;

  /* Check for zero, nan and infinity. */
  if (x != x || x + x == x )
    {
      *exp = 0;
      return x;
    }

  if (x < 0)
    return -frexpl(-x, exp);

  exponent = 0;
  if (x > 1.0)
    {
      for (next = exponents, exponents[0] = 2.0L, bit = 1;
	   *next <= x + x;
	   bit <<= 1, next[1] = next[0] * next[0], next++);

      for (; next >= exponents; bit >>= 1, next--)
	if (x + x >= *next)
	  {
	    x /= *next;
	    exponent |= bit;
	  }

    }

  else if (x < 0.5)
    {
      for (next = exponents, exponents[0] = 0.5L, bit = 1;
	   *next > x;
	   bit <<= 1, next[1] = next[0] * next[0], next++);

      for (; next >= exponents; bit >>= 1, next--)
	if (x < *next)
	  {
	    x /= *next;
	    exponent |= bit;
	  }

      exponent = -exponent;
    }

  *exp = exponent;
  return x;
}

#if 0
int
main()
{
  long double x;
  int y;
  x = frexpl(0.0L / 0.0L, &y); printf ("%.6Lg %d\n", x, y);
  x = frexpl(1.0L / 0.0L, &y); printf ("%.6Lg %d\n", x, y);
  x = frexpl(-1.0L / 0.0L, &y); printf ("%.6Lg %d\n", x, y);
  x = frexpl(0.5L, &y); printf ("%.6Lg %d\n", x, y);
  x = frexpl(0.75L, &y); printf ("%.6Lg %d\n", x, y);
  x = frexpl(3.6L, &y); printf ("%.6Lg %d\n", x, y);
  x = frexpl(17.8L, &y); printf ("%.6Lg %d\n", x, y);
  x = frexpl(8.0L, &y); printf ("%.6Lg %d\n", x, y);
  x = frexpl(0.3L, &y); printf ("%.6Lg %d\n", x, y);
  x = frexpl(0.49L, &y); printf ("%.6Lg %d\n", x, y);
  x = frexpl(0.049L, &y); printf ("%.6Lg %d\n", x, y);
  x = frexpl(0.0245L, &y); printf ("%.6Lg %d\n", x, y);
  x = frexpl(0.0625L, &y); printf ("%.6Lg %d\n", x, y);
}
#endif

