/* Test of rounding towards negative infinity.
   Copyright (C) 2007-2008 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2007.  */

#include <config.h>

#include <math.h>

#include <float.h>
#include <stdio.h>
#include <stdlib.h>

#include "fpucw.h"
#include "isnanl-nolibm.h"

#define ASSERT(expr) \
  do									     \
    {									     \
      if (!(expr))							     \
        {								     \
          fprintf (stderr, "%s:%d: assertion failed\n", __FILE__, __LINE__); \
          fflush (stderr);						     \
          abort ();							     \
        }								     \
    }									     \
  while (0)

/* On HP-UX 10.20, negating 0.0L does not yield -0.0L.
   So we use minus_zero instead.
   Note that the expression -LDBL_MIN * LDBL_MIN does not work on other
   platforms, such as when cross-compiling to PowerPC on MacOS X 10.5.  */
#if defined __hpux || defined __sgi
long double minus_zero = -LDBL_MIN * LDBL_MIN;
#else
long double minus_zero = -0.0L;
#endif

int
main ()
{
  DECL_LONG_DOUBLE_ROUNDING

  BEGIN_LONG_DOUBLE_ROUNDING ();

  /* Zero.  */
  ASSERT (floorl (0.0L) == 0.0L);
  ASSERT (floorl (minus_zero) == 0.0L);
  /* Positive numbers.  */
  ASSERT (floorl (0.3L) == 0.0L);
  ASSERT (floorl (0.7L) == 0.0L);
  ASSERT (floorl (1.0L) == 1.0L);
  ASSERT (floorl (1.5L) == 1.0L);
  ASSERT (floorl (1.999L) == 1.0L);
  ASSERT (floorl (2.0L) == 2.0L);
  ASSERT (floorl (65535.999L) == 65535.0L);
  ASSERT (floorl (65536.0L) == 65536.0L);
  ASSERT (floorl (2.341e31L) == 2.341e31L);
  /* Negative numbers.  */
  ASSERT (floorl (-0.3L) == -1.0L);
  ASSERT (floorl (-0.7L) == -1.0L);
  ASSERT (floorl (-1.0L) == -1.0L);
  ASSERT (floorl (-1.001L) == -2.0L);
  ASSERT (floorl (-1.5L) == -2.0L);
  ASSERT (floorl (-1.999L) == -2.0L);
  ASSERT (floorl (-2.0L) == -2.0L);
  ASSERT (floorl (-65535.999L) == -65536.0L);
  ASSERT (floorl (-65536.0L) == -65536.0L);
  ASSERT (floorl (-2.341e31L) == -2.341e31L);
  /* Infinite numbers.  */
  ASSERT (floorl (1.0L / 0.0L) == 1.0L / 0.0L);
  ASSERT (floorl (-1.0L / 0.0L) == -1.0L / 0.0L);
  /* NaNs.  */
  ASSERT (isnanl (floorl (0.0L / 0.0L)));

  return 0;
}
