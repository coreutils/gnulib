/* Test setpayloadsig.
   Copyright 2024-2025 Free Software Foundation, Inc.

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

/* Specification.  */
#include <math.h>

#include "signature.h"
SIGNATURE_CHECK (setpayloadsig, int, (double *, double));

#include "infinity.h"
#include "isnand-nolibm.h"
#include "macros.h"

#define PAYLOAD_BITS (53 - 2) /* = (DBL_MANT_DIG - 2) */

int
main ()
{
  int i;
  double p;

  {
    /* Test valid payloads.  */
    for (i = 0, p = 1.0; i < PAYLOAD_BITS; i++, p *= 2.0)
      {
        int ret;
        double x;

        ret = setpayloadsig (&x, p);
        ASSERT (ret == 0);
        ASSERT (isnand (x));
      }
    /* Test out-of-range payload.  */
    int ret;
    double x;

    ret = setpayloadsig (&x, p);
    ASSERT (ret != 0);
    ASSERT (x == 0.0);
  }

  /* Test infinite payload.  */
  {
    int ret;
    double x;

    ret = setpayloadsig (&x, Infinityd ());
    ASSERT (ret != 0);
    ASSERT (x == 0.0);
  }

  /* Test negative payload.  */
  {
    int ret;
    double x;

    ret = setpayloadsig (&x, -1.0);
    ASSERT (ret != 0);
    ASSERT (x == 0.0);
  }

  /* Test fractional payload.  */
  {
    int ret;
    double x;

    ret = setpayloadsig (&x, 1.4);
    ASSERT (ret != 0);
    ASSERT (x == 0.0);
  }

  return test_exit_status;
}
