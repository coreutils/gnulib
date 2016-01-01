/* Absolute value.
   Copyright (C) 2012-2016 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include <config.h>

/* Specification.  */
#include <math.h>

/* fabsl (x) can be defined as copysignl (x, 1.0L).
   Or, more directly, as (signbit (x) ? - x : x).  */

#if HAVE_SAME_LONG_DOUBLE_AS_DOUBLE

long double
fabsl (long double x)
{
  return fabs (x);
}

#else

long double
fabsl (long double x)
{
  return (signbit (x) ? - x : x);
}

#endif
