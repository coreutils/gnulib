/* Construct a quiet NaN 'double' with a given payload.
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
setpayload (double *result, double payload)
{
#if DBL_MANT_DIG == 53
  if (
# if defined __hppa || (defined __mips__ && !MIPS_NAN2008_DOUBLE) || defined __sh__
      /* A zero payload is not allowed, because that would denote Infinity.
         Cf. snan.h.  */
      payload > 0.0
# else
      payload >= 0.0
# endif
      && payload < 2251799813685248.0 /* (double) (1ULL << (DBL_MANT_DIG - 2)) */
      && payload == (double) (int64_t) payload)
    {
      union { uint64_t i; double f; } x;
      x.f = positive_NaNd ();
      x.i = (x.i & ~(((uint64_t) 1 << (DBL_MANT_DIG - 2)) - 1))
            | (int64_t) payload;
      *result = x.f;
      return 0;
    }
  else
    {
      *result = 0.0;
      return -1;
    }
#else
# error "Please port gnulib setpayload.c to your platform!"
#endif
}
