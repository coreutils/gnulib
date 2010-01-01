/* Test of rounding to nearest, breaking ties away from zero.
   Copyright (C) 2007-2010 Free Software Foundation, Inc.

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

/* Written by Ben Pfaff <blp@gnu.org>, 2007.
   Based heavily on Bruno Haible's test-truncl.c. */

#include <config.h>

#include <math.h>

#include "signature.h"
SIGNATURE_CHECK (roundl, long double, (long double));

#include <float.h>

#include "fpucw.h"
#include "isnanl-nolibm.h"
#include "nan.h"
#include "macros.h"

/* On HP-UX 10.20, negating 0.0L does not yield -0.0L.
   So we use minus_zero instead.
   IRIX cc can't put -0.0L into .data, but can compute at runtime.
   Note that the expression -LDBL_MIN * LDBL_MIN does not work on other
   platforms, such as when cross-compiling to PowerPC on MacOS X 10.5.  */
#if defined __hpux || defined __sgi
static long double
compute_minus_zero (void)
{
  return -LDBL_MIN * LDBL_MIN;
}
# define minus_zero compute_minus_zero ()
#else
long double minus_zero = -0.0L;
#endif

int
main ()
{
  DECL_LONG_DOUBLE_ROUNDING

  BEGIN_LONG_DOUBLE_ROUNDING ();

  /* Zero.  */
  ASSERT (roundl (0.0L) == 0.0L);
  ASSERT (roundl (minus_zero) == 0.0L);
  /* Positive numbers.  */
  ASSERT (roundl (0.3L) == 0.0L);
  ASSERT (roundl (0.5L) == 1.0L);
  ASSERT (roundl (0.7L) == 1.0L);
  ASSERT (roundl (1.0L) == 1.0L);
  ASSERT (roundl (1.5L) == 2.0L);
  ASSERT (roundl (2.5L) == 3.0L);
  ASSERT (roundl (1.999L) == 2.0L);
  ASSERT (roundl (2.0L) == 2.0L);
  ASSERT (roundl (65535.999L) == 65536.0L);
  ASSERT (roundl (65536.0L) == 65536.0L);
  ASSERT (roundl (65536.001L) == 65536.0L);
  ASSERT (roundl (2.341e31L) == 2.341e31L);
  /* Negative numbers.  */
  ASSERT (roundl (-0.3L) == 0.0L);
  ASSERT (roundl (-0.5L) == -1.0L);
  ASSERT (roundl (-0.7L) == -1.0L);
  ASSERT (roundl (-1.0L) == -1.0L);
  ASSERT (roundl (-1.5L) == -2.0L);
  ASSERT (roundl (-2.5L) == -3.0L);
  ASSERT (roundl (-1.999L) == -2.0L);
  ASSERT (roundl (-2.0L) == -2.0L);
  ASSERT (roundl (-65535.999L) == -65536.0L);
  ASSERT (roundl (-65536.0L) == -65536.0L);
  ASSERT (roundl (-65536.001L) == -65536.0L);
  ASSERT (roundl (-2.341e31L) == -2.341e31L);
  /* Infinite numbers.  */
  ASSERT (roundl (1.0 / 0.0L) == 1.0 / 0.0L);
  ASSERT (roundl (-1.0 / 0.0L) == -1.0 / 0.0L);
  /* NaNs.  */
  ASSERT (isnanl (roundl (NaNl ())));

  return 0;
}
