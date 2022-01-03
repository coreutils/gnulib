/* Base 10 logarithmic function.
   Copyright (C) 2012-2022 Free Software Foundation, Inc.

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
log10l (long double x)
{
  return log10 (x);
}

#elif 0 /* was: HAVE_LOG10L */

# include <float.h>

long double
log10l (long double x)
# undef log10l
{
  /* Work around the OSF/1 5.1 bug.  */
  if (x == 0.0L)
    /* Return -Infinity.  */
    return -1.0L / 0.0L;
  {
    long double y = log10l (x);
# ifdef __sgi
    /* Normalize the +Infinity value.  */
    if (y > LDBL_MAX)
      y = 1.0L / 0.0L;
# endif
    return y;
  }
}

#else

/* 1 / log(10) */
static const long double inv_log10 =
  0.43429448190325182765112891891660508229439700580366656611445378316586464920887L;

long double
log10l (long double x)
{
  return logl (x) * inv_log10;
}

#endif
