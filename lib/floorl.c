/* Emulation for floorl.
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

#include "mathl.h"

/* To compute the integer part of X, sum a big enough
   integer so that the precision of the floating point
   number is exactly 1.  */

long double
floorl(long double x)
{
  long double y;
  if (x < 0.0L)
    y = -(1.0L / LDBL_EPSILON - x - 1.0 / LDBL_EPSILON);
  else
    y = 1.0L / LDBL_EPSILON + x - 1.0 / LDBL_EPSILON;

  if (y > x)
    return y - 1.0L;
  else
    return y;
}
