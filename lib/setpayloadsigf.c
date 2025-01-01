/* Construct a signalling NaN 'float' with a given payload.
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
#include <string.h>

#include "signed-snan.h"

int
setpayloadsigf (float *result, float payload)
{
#if FLT_MANT_DIG == 24
  if (
# if defined __hppa || (defined __mips__ && !MIPS_NAN2008_FLOAT) || defined __sh__
      payload >= 0.0f
# else
      /* A zero payload is not allowed, because that would denote Infinity.
         Cf. snan.h.  */
      payload > 0.0f
# endif
      && payload < 4194304.0f /* (float) (1U << (FLT_MANT_DIG - 2)) */
      && payload == (float) (int32_t) payload)
    {
      memory_float x = memory_positive_SNaNf ();
      x.word[0] = (x.word[0] & ~((1U << (FLT_MANT_DIG - 2)) - 1))
                  | (int32_t) payload;
# if 0
      *result = x.value;
# else
      /* On 32-bit x86 processors, as well as on x86_64 processors with
         CC="gcc -mfpmath=387", the evaluation of *x and *y above is done
         through an 'flds' instruction, which converts a signalling NaN to
         a quiet NaN. See
         <https://lists.gnu.org/archive/html/bug-gnulib/2023-10/msg00060.html>
         for details.  Use memcpy to avoid this.  */
      memcpy (result, &x.value, sizeof (float));
# endif
      return 0;
    }
  else
    {
      *result = 0.0f;
      return -1;
    }
#else
# error "Please port gnulib setpayloadsigf.c to your platform!"
#endif
}
