/* Round towards negative infinity.
   Copyright (C) 2007, 2009-2022 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2007.  */

#include <config.h>

#if HAVE_SAME_LONG_DOUBLE_AS_DOUBLE

/* Specification.  */
# include <math.h>

long double
floorl (long double x)
{
  return floor (x);
}

#else

# define USE_LONG_DOUBLE
# include "floor.c"

#endif
