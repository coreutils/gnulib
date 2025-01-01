/* Test getpayloadf.
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
SIGNATURE_CHECK (getpayloadf, float, (const float *));

#include "minus-zero.h"
#include "infinity.h"
#include "signed-nan.h"
#include "signed-snan.h"
#include "macros.h"

#define PAYLOAD_BITS (24 - 2) /* = (FLT_MANT_DIG - 2) */

int
main ()
{
  float arg;
  float ret;

  /* Test non-NaN arguments.  */

  arg = 2.718281828459045f;
  ret = getpayloadf (&arg);
  ASSERT (ret == -1.0f);

  arg = -3.141592653589793f;
  ret = getpayloadf (&arg);
  ASSERT (ret == -1.0f);

  arg = 0.0f;
  ret = getpayloadf (&arg);
  ASSERT (ret == -1.0f);

  arg = minus_zerof;
  ret = getpayloadf (&arg);
  ASSERT (ret == -1.0f);

  arg = Infinityf ();
  ret = getpayloadf (&arg);
  ASSERT (ret == -1.0f);

  arg = - Infinityf ();
  ret = getpayloadf (&arg);
  ASSERT (ret == -1.0f);

  /* Test quiet NaNs.  */
  {
    int i;
    float p;

    for (i = 0, p = 1.0f; i < PAYLOAD_BITS; i++, p *= 2.0f)
      {
        ASSERT (setpayloadf (&arg, p) == 0);
        ret = getpayloadf (&arg);
        ASSERT (ret == p);
        /* Test quiet NaNs with sign bit == 1.  */
        arg = minus_NaNf (arg);
        ret = getpayloadf (&arg);
        ASSERT (ret == p);
      }

    p = 2300902.0f;
    ASSERT (setpayloadf (&arg, p) == 0);
    ret = getpayloadf (&arg);
    ASSERT (ret == p);
  }

  /* Test signalling NaNs.  */
  {
    int i;
    float p;

    for (i = 0, p = 1.0f; i < PAYLOAD_BITS; i++, p *= 2.0f)
      {
        ASSERT (setpayloadsigf (&arg, p) == 0);
        ret = getpayloadf (&arg);
        ASSERT (ret == p);
      }

    p = 2300902.0f;
    ASSERT (setpayloadsigf (&arg, p) == 0);
    ret = getpayloadf (&arg);
    ASSERT (ret == p);

    /* Test signalling NaNs with sign bit == 1.  */
    memory_float pos_arg = memory_positive_SNaNf ();
    memory_float neg_arg = memory_negative_SNaNf ();
    float pos_ret = getpayloadf (&pos_arg.value);
    float neg_ret = getpayloadf (&neg_arg.value);
    ASSERT (neg_ret == pos_ret);
  }

  return test_exit_status;
}
