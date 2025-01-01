/* Test setpayloadf.
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
SIGNATURE_CHECK (setpayloadf, int, (float *, float));

#include "infinity.h"
#include "isnanf-nolibm.h"
#include "macros.h"

#define PAYLOAD_BITS (24 - 2) /* = (FLT_MANT_DIG - 2) */

int
main ()
{
  int i;
  float p;

  {
    /* Test valid payloads.  */
    for (i = 0, p = 1.0f; i < PAYLOAD_BITS; i++, p *= 2.0f)
      {
        int ret;
        float x;

        ret = setpayloadf (&x, p);
        ASSERT (ret == 0);
        ASSERT (isnanf (x));
      }
    /* Test out-of-range payload.  */
    int ret;
    float x;

    ret = setpayloadf (&x, p);
    ASSERT (ret != 0);
    ASSERT (x == 0.0f);
  }

  /* Test infinite payload.  */
  {
    int ret;
    float x;

    ret = setpayloadf (&x, Infinityf ());
    ASSERT (ret != 0);
    ASSERT (x == 0.0f);
  }

  /* Test negative payload.  */
  {
    int ret;
    float x;

    ret = setpayloadf (&x, -1.0f);
    ASSERT (ret != 0);
    ASSERT (x == 0.0f);
  }

  /* Test fractional payload.  */
  {
    int ret;
    float x;

    ret = setpayloadf (&x, 1.4f);
    ASSERT (ret != 0);
    ASSERT (x == 0.0f);
  }

  return test_exit_status;
}
