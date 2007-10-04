/* Round towards zero.
   Copyright (C) 2007 Free Software Foundation, Inc.

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
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

/* Written by Bruno Haible <bruno@clisp.org>, 2007.  */

#include <config.h>

/* Specification.  */
#include <math.h>

#include <float.h>

/* 2^(DBL_MANT_DIG-1).  */
static const double TWO_MANT_DIG =
  /* Assume DBL_MANT_DIG <= 4 * 31.
     Use the identity
       n = floor(n/4) + floor((n+1)/4) + floor((n+2)/4) + floor((n+3)/4).  */
  (double) (1U << ((DBL_MANT_DIG - 1) / 4))
  * (double) (1U << ((DBL_MANT_DIG - 1 + 1) / 4))
  * (double) (1U << ((DBL_MANT_DIG - 1 + 2) / 4))
  * (double) (1U << ((DBL_MANT_DIG - 1 + 3) / 4));

double
trunc (double x)
{
  /* The use of 'volatile' guarantees that excess precision bits are dropped
     at each addition step and before the following comparison at the caller's
     site.  It is necessary on x86 systems where double-floats are not IEEE
     compliant by default, to avoid that the results become platform and compiler
     option dependent.  'volatile' is a portable alternative to gcc's
     -ffloat-store option.  */
  volatile double y = x;
  volatile double z = y;

  if (z > 0)
    {
      /* Round to the next integer (nearest or up or down, doesn't matter).  */
      z += TWO_MANT_DIG;
      z -= TWO_MANT_DIG;
      /* Enforce rounding down.  */
      if (z > y)
	z -= 1.0;
    }
  else if (z < 0)
    {
      /* Round to the next integer (nearest or up or down, doesn't matter).  */
      z -= TWO_MANT_DIG;
      z += TWO_MANT_DIG;
      /* Enforce rounding up.  */
      if (z < y)
	z += 1.0;
    }
  return z;
}
