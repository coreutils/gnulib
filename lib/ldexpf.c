/* Multiply a 'float' by a power of 2.
   Copyright (C) 2011-2016 Free Software Foundation, Inc.

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

/* Avoid some warnings from "gcc -Wshadow".
   This file doesn't use the exp() function.  */
#undef exp
#define exp exponent

float
ldexpf (float x, int exp)
{
  return (float) ldexp ((double) x, exp);
}
