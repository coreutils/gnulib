/* Test getpayload.
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
SIGNATURE_CHECK (getpayload, double, (const double *));

#include "minus-zero.h"
#include "infinity.h"
#include "signed-nan.h"
#include "signed-snan.h"
#include "macros.h"

#define PAYLOAD_BITS (53 - 2) /* = (DBL_MANT_DIG - 2) */

int
main ()
{
  double arg;
  double ret;

  /* Test non-NaN arguments.  */

  arg = 2.718281828459045;
  ret = getpayload (&arg);
  ASSERT (ret == -1.0);

  arg = -3.141592653589793;
  ret = getpayload (&arg);
  ASSERT (ret == -1.0);

  arg = 0.0;
  ret = getpayload (&arg);
  ASSERT (ret == -1.0);

  arg = minus_zerod;
  ret = getpayload (&arg);
  ASSERT (ret == -1.0);

  arg = Infinityd ();
  ret = getpayload (&arg);
  ASSERT (ret == -1.0);

  arg = - Infinityd ();
  ret = getpayload (&arg);
  ASSERT (ret == -1.0);

  /* Test quiet NaNs.  */
  {
    int i;
    double p;

    for (i = 0, p = 1.0; i < PAYLOAD_BITS; i++, p *= 2.0)
      {
        ASSERT (setpayload (&arg, p) == 0);
        ret = getpayload (&arg);
        ASSERT (ret == p);
        /* Test quiet NaNs with sign bit == 1.  */
        arg = minus_NaNd (arg);
        ret = getpayload (&arg);
        ASSERT (ret == p);
      }

    p = 1320699239819071.0;
    ASSERT (setpayload (&arg, p) == 0);
    ret = getpayload (&arg);
    ASSERT (ret == p);
  }

  /* Test signalling NaNs.  */
  {
    int i;
    double p;

    for (i = 0, p = 1.0; i < PAYLOAD_BITS; i++, p *= 2.0)
      {
        ASSERT (setpayloadsig (&arg, p) == 0);
        ret = getpayload (&arg);
        ASSERT (ret == p);
      }

    p = 1320699239819071.0;
    ASSERT (setpayloadsig (&arg, p) == 0);
    ret = getpayload (&arg);
    ASSERT (ret == p);

    /* Test signalling NaNs with sign bit == 1.  */
    memory_double pos_arg = memory_positive_SNaNd ();
    memory_double neg_arg = memory_negative_SNaNd ();
    double pos_ret = getpayload (&pos_arg.value);
    double neg_ret = getpayload (&neg_arg.value);
    ASSERT (neg_ret == pos_ret);
  }

  return test_exit_status;
}
