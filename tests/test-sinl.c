/* Test of sinl() function.
   Copyright (C) 2010-2025 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2010.  */

#include <config.h>

#include <math.h>

#include "signature.h"
SIGNATURE_CHECK (sinl, long double, (long double));

#include "fpucw.h"
#include "macros.h"

volatile long double x;
long double y;

int
main ()
{
  DECL_LONG_DOUBLE_ROUNDING

  BEGIN_LONG_DOUBLE_ROUNDING ();

  /* A particular value.  */
  x = 0.6L;
  y = sinl (x);
  ASSERT (y >= 0.5646424733L && y <= 0.5646424734L);

  /* A small negative value.  */
  x = -0.1L;
  y = sinl (x);
  ASSERT (y >= -0.09983341665L && y <= -0.09983341664L);

  /* A very small negative value.  */
  x = -0.000000000000000004L;
  y = sinl (x);
  ASSERT (y >= -0.000000000000000004000000000L
          && y <= -0.000000000000000003999999999L);

  return test_exit_status;
}
