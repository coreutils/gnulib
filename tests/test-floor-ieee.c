/* Test of rounding towards negative infinity.
   Copyright (C) 2010 Free Software Foundation, Inc.

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

#include <math.h>

#include "minus-zero.h"
#include "macros.h"

int
main ()
{
  /* Zero.  */
  ASSERT (!signbit (floor (0.0)));
  ASSERT (!!signbit (floor (minus_zerod)) == !!signbit (minus_zerod));

  return 0;
}
