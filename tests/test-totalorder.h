/* Test a totalorder-like function.
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

#include "infinity.h"
#include "macros.h"
#include "minus-zero.h"
#include "signed-nan.h"

int
main ()
{
  TOTALORDER_TYPE x[] =
    {
      TOTALORDER_NEGATIVE_NAN (), -TOTALORDER_INF (), -1e37, -1, -1e-5,
      TOTALORDER_MINUS_ZERO, 0,
      1e-5, 1, 1e37, TOTALORDER_INF (), TOTALORDER_POSITIVE_NAN ()
    };
  int n = sizeof x / sizeof *x;

  for (int i = 0; i < n; i++)
    for (int j = 0; j < n; j++)
      ASSERT (!!TOTALORDER (&x[i], &x[j]) == (i <= j));
  return 0;
}
