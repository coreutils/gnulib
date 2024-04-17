/* Test a totalorder-like function.
   Copyright 2023-2024 Free Software Foundation, Inc.

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

#include <stdio.h>

#include <math.h>
#include "infinity.h"
#include "macros.h"
#include "minus-zero.h"
#include "signed-nan.h"
#include "signed-snan.h"

static TOTALORDER_TYPE
positive_NaN_with_payload (int payload)
{
  TOTALORDER_TYPE x;
  ASSERT (TOTALORDER_SETPAYLOAD (&x.value, payload) == 0);
  return x;
}

static TOTALORDER_TYPE
negative_NaN_with_payload (int payload)
{
  TOTALORDER_TYPE x;
  ASSERT (TOTALORDER_SETPAYLOAD (&x.value, payload) == 0);
  x.value = - x.value;
  return x;
}

int
main ()
{
  TOTALORDER_TYPE x[] =
    {
      negative_NaN_with_payload (1729),
      negative_NaN_with_payload (641),
#if TOTALORDER_HAVE_SNAN
      TOTALORDER_NEGATIVE_SNAN (),
#endif
      { -TOTALORDER_INF () },
      { -1e37 },
      { -1 },
      { -1e-5 },
      { TOTALORDER_MINUS_ZERO },
      { 0 },
      { 1e-5 },
      { 1 },
      { 1e37 },
      { TOTALORDER_INF () },
#if TOTALORDER_HAVE_SNAN
      TOTALORDER_POSITIVE_SNAN (),
#endif
      positive_NaN_with_payload (641),
      positive_NaN_with_payload (1729)
    };
  int n = SIZEOF (x);
  int result = 0;

  for (int i = 0; i < n; i++)
    for (int j = 0; j < n; j++)
      if (!(!!TOTALORDER (&x[i].value, &x[j].value) == (i <= j)))
        {
          fprintf (stderr, "Failed: i=%d j=%d\n", i, j);
          result = 1;
        }

  return result;
}
