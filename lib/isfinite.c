/* Test for finite value (zero, subnormal, or normal, and not infinite or NaN).
   Copyright (C) 2007-2022 Free Software Foundation, Inc.

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

/* Written by Ben Pfaff <blp@gnu.org>, 2007. */

#include <config.h>

#include "isnanf-nolibm.h"
#include "isnand-nolibm.h"
#include "isnanl-nolibm.h"

/* The "cc" compiler on HP-UX 11.11, when optimizing, simplifies the test
   x - y == 0.0  to  x == y, a simplification which is invalid when x and y
   are Infinity.  Disable this optimization.  */
#if defined __hpux && !defined __GNUC__
static float zerof;
static double zerod;
static long double zerol;
#else
# define zerof 0.f
# define zerod 0.
# define zerol 0.L
#endif

int gl_isfinitef (float x)
{
  return !isnanf (x) && x - x == zerof;
}

int gl_isfinited (double x)
{
  return !isnand (x) && x - x == zerod;
}

int gl_isfinitel (long double x)
{
  return !isnanl (x) && x - x == zerol;
}
