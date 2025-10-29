/* Test setpayloadsigl.
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
SIGNATURE_CHECK (setpayloadsigl, int, (long double *, long double));

#include <float.h>

#include "infinity.h"
#include "isnanl-nolibm.h"
#include "macros.h"

#if defined _ARCH_PPC && LDBL_MANT_DIG == 106
  /* This is PowerPC "double double", a pair of two doubles.  NaN is represented
     as the corresponding 64-bit IEEE value in the first double; the second is
     irrelevant and therefore does not contain a payload.  */
# define PAYLOAD_BITS (DBL_MANT_DIG - 2)
#else
# define PAYLOAD_BITS (LDBL_MANT_DIG - 2)
#endif

int
main ()
{
  int i;
  long double p;

  {
    /* Test valid payloads.  */
    for (i = 0, p = 1.0L; i < PAYLOAD_BITS; i++, p *= 2.0L)
      {
        int ret;
        long double x;

        ret = setpayloadsigl (&x, p);
        ASSERT (ret == 0);
        ASSERT (isnanl (x));
      }
    /* Test out-of-range payload.  */
    int ret;
    long double x;

    ret = setpayloadsigl (&x, p);
    ASSERT (ret != 0);
    ASSERT (x == 0.0L);
  }

  /* Test infinite payload.  */
  {
    int ret;
    long double x;

    ret = setpayloadsigl (&x, Infinityl ());
    ASSERT (ret != 0);
    ASSERT (x == 0.0L);
  }

  /* Test negative payload.  */
  {
    int ret;
    long double x;

    ret = setpayloadsigl (&x, -1.0L);
    ASSERT (ret != 0);
    ASSERT (x == 0.0L);
  }

  /* Test fractional payload.  */
  {
    int ret;
    long double x;

    ret = setpayloadsigl (&x, 1.4L);
    ASSERT (ret != 0);
    ASSERT (x == 0.0L);
  }

  return test_exit_status;
}
