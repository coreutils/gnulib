/* Test totalorder.
   Copyright 2023 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#include <config.h>

#include <math.h>

#include "infinity.h"
#include "macros.h"
#include "minus-zero.h"
#include "nan.h"

#ifndef TOTALORDER
# define TOTALORDER totalorder
# define TOTALORDER_INF Infinityd
# define TOTALORDER_MINUS_ZERO minus_zerod
# define TOTALORDER_NAN NaNd
# define TOTALORDER_TYPE double
#endif

int
main ()
{
  TOTALORDER_TYPE x[] =
    {
      -TOTALORDER_NAN (), -TOTALORDER_INF (), -1e37, -1, -1e-5,
      TOTALORDER_MINUS_ZERO, 0,
      1e-5, 1, 1e37, TOTALORDER_INF (), TOTALORDER_NAN ()
    };
  int n = sizeof x / sizeof *x;

  /* TOTALORDER_NAN () yields a NaN of unknown sign, so fix the
     first NaN to be negative and the last NaN to be nonnegative.
     Do this via volatile accesses, to work around GCC bug 111655.  */
  TOTALORDER_TYPE volatile a = x[0], z = x[n - 1];
  if (! signbit (a)) a = -a;
  if (  signbit (z)) z = -z;
  x[0] = a, x[n - 1] = z;

  for (int i = 0; i < n; i++)
    for (int j = 0; j < n; j++)
      ASSERT (!!TOTALORDER (&x[i], &x[j]) == (i <= j));
  return 0;
}
