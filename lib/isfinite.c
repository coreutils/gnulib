/* Test for finite value (zero, subnormal, or normal, and not infinite or NaN).
   Copyright (C) 2007-2010 Free Software Foundation, Inc.

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

/* Written by Ben Pfaff <blp@gnu.org>, 2007. */

#include <config.h>

#include "isnanf-nolibm.h"
#include "isnand-nolibm.h"
#include "isnanl-nolibm.h"

int gl_isfinitef (float x)
{
  return !isnanf (x) && x - x == 0.f;
}

int gl_isfinited (double x)
{
  return !isnand (x) && x - x == 0.;
}

int gl_isfinitel (long double x)
{
  return !isnanl (x) && x - x == 0.L;
}
