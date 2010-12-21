/* Test of rounding to nearest, breaking ties away from zero.
   Copyright (C) 2010 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

#include <config.h>

#include <math.h>

#include "minus-zero.h"
#include "macros.h"

int
main ()
{
  /* Zero.  */
  ASSERT (!signbit (roundf (0.0f)));
  ASSERT (!!signbit (roundf (minus_zerof)) == !!signbit (minus_zerof));

  return 0;
}
