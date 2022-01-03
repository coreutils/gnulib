/* Copy sign into another 'long double' number.
   Copyright (C) 2011-2022 Free Software Foundation, Inc.

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

#include <config.h>

/* Specification.  */
#include <math.h>

#if HAVE_SAME_LONG_DOUBLE_AS_DOUBLE

long double
copysignl (long double x, long double y)
{
  return copysign (x, y);
}

#else

# if defined __hpux && !defined __GNUC__

#  include <float.h>

/* HP cc on HP-UX 10.20 has a bug with the constant expression -0.0L.  */
static long double
compute_minus_zerol (void)
{
  return -LDBL_MIN * LDBL_MIN;
}
#  define minus_zerol compute_minus_zerol ()

/* HP cc on HP-UX 11 has a bug: When x is a positive zero, - x comes out
   as a positive zero, rather than as a minus zero.  Work around it.  */
static long double
unary_minus (long double x)
{
  if (x == 0.0L)
    {
      if (signbit (x))
        return 0.0L;
      else
        return minus_zerol;
    }
  else
    return - x;
}

# endif

long double
copysignl (long double x, long double y)
{
# if defined __hpux && !defined __GNUC__
  return (signbit (x) != signbit (y) ? unary_minus (x) : x);
# else
  return (signbit (x) != signbit (y) ? - x : x);
# endif
}

#endif
