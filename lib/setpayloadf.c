/* Construct a quiet NaN 'float' with a given payload.
   Copyright 2024-2025 Free Software Foundation, Inc.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Written by Bruno Haible.  */

#include <config.h>

/* Specification.  */
#include <math.h>

#include <float.h>
#include <stdint.h>

#include "signed-nan.h"

int
setpayloadf (float *result, float payload)
{
#if FLT_MANT_DIG == 24
  if (
# if defined __hppa || (defined __mips__ && !MIPS_NAN2008_FLOAT) || defined __sh__
      /* A zero payload is not allowed, because that would denote Infinity.
         Cf. snan.h.  */
      payload > 0.0f
# else
      payload >= 0.0f
# endif
      && payload < 4194304.0f /* (float) (1U << (FLT_MANT_DIG - 2)) */
      && payload == (float) (int32_t) payload)
    {
      union { uint32_t i; float f; } x;
      x.f = positive_NaNf ();
      x.i = (x.i & ~(((uint32_t) 1 << (FLT_MANT_DIG - 2)) - 1))
            | (int32_t) payload;
      *result = x.f;
      return 0;
    }
  else
    {
      *result = 0.0f;
      return -1;
    }
#else
# error "Please port gnulib setpayloadf.c to your platform!"
#endif
}
