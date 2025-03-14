/* Test of fma().
   Copyright (C) 2011-2025 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2011.  */

#include <config.h>

#include <math.h>

#include "signature.h"
SIGNATURE_CHECK (fma, double, (double, double, double));

#include "isnand-nolibm.h"
#include "infinity.h"
#include "nan.h"
#include "macros.h"

#undef INFINITY
#undef NAN

#define DOUBLE double
#define ISNAN isnand
#define INFINITY Infinityd ()
#define NAN NaNd ()
#define L_(literal) literal
#include "test-fma1.h"

int
main ()
{
  test_function (fma);

  return test_exit_status;
}
