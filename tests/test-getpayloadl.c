/* Test getpayloadl.
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
SIGNATURE_CHECK (getpayloadl, long double, (const long double *));

#include "minus-zero.h"
#include "infinity.h"
#include "signed-nan.h"
#include "signed-snan.h"
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
  long double arg;
  long double ret;

  /* Test non-NaN arguments.  */

  arg = 2.71828182845904523536028747135266249776L;
  ret = getpayloadl (&arg);
  ASSERT (ret == -1.0L);

  arg = -3.1415926535897932384626433832795028842L;
  ret = getpayloadl (&arg);
  ASSERT (ret == -1.0L);

  arg = 0.0L;
  ret = getpayloadl (&arg);
  ASSERT (ret == -1.0L);

  arg = minus_zerol;
  ret = getpayloadl (&arg);
  ASSERT (ret == -1.0L);

  arg = Infinityl ();
  ret = getpayloadl (&arg);
  ASSERT (ret == -1.0L);

  arg = - Infinityl ();
  ret = getpayloadl (&arg);
  ASSERT (ret == -1.0L);

  /* Test quiet NaNs.  */
  {
    int i;
    long double p;

    for (i = 0, p = 1.0L; i < PAYLOAD_BITS; i++, p *= 2.0L)
      {
        ASSERT (setpayloadl (&arg, p) == 0);
        ret = getpayloadl (&arg);
        ASSERT (ret == p);
        /* Test quiet NaNs with sign bit == 1.  */
        arg = minus_NaNl (arg);
        ret = getpayloadl (&arg);
        ASSERT (ret == p);
      }

    p = 1320699239819071.0L;
    ASSERT (setpayloadl (&arg, p) == 0);
    ret = getpayloadl (&arg);
    ASSERT (ret == p);
  }

  /* Test signalling NaNs.  */
  {
    int i;
    long double p;

    for (i = 0, p = 1.0L; i < PAYLOAD_BITS; i++, p *= 2.0L)
      {
        ASSERT (setpayloadsigl (&arg, p) == 0);
        ret = getpayloadl (&arg);
        ASSERT (ret == p);
      }

    p = 1320699239819071.0L;
    ASSERT (setpayloadsigl (&arg, p) == 0);
    ret = getpayloadl (&arg);
    ASSERT (ret == p);

    /* Test signalling NaNs with sign bit == 1.  */
    memory_long_double pos_arg = memory_positive_SNaNl ();
    memory_long_double neg_arg = memory_negative_SNaNl ();
    long double pos_ret = getpayloadl (&pos_arg.value);
    long double neg_ret = getpayloadl (&neg_arg.value);
    ASSERT (neg_ret == pos_ret);
  }

  return test_exit_status;
}
