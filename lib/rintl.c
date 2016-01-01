/* Round according to the current rounding mode.
   Copyright (C) 2011-2016 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, see <http://www.gnu.org/licenses/>.  */

#include <config.h>

#if HAVE_SAME_LONG_DOUBLE_AS_DOUBLE

/* Specification.  */
# include <math.h>

long double
rintl (long double x)
{
  return rint (x);
}

#else

# define USE_LONG_DOUBLE
# include "rint.c"

#endif
