/* Construct a signalling NaN 'double' with a given payload.
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
setpayloadsig (double *result, double payload)
{
#if DBL_MANT_DIG == 53
  if (
# if defined __hppa || (defined __mips__ && !MIPS_NAN2008_DOUBLE) || defined __sh__
      payload >= 0.0
# else
      /* A zero payload is not allowed, because that would denote Infinity.
         Cf. snan.h.  */
      payload > 0.0
# endif
      && payload < 2251799813685248.0 /* (double) (1ULL << (DBL_MANT_DIG - 2)) */
      && payload == (double) (int64_t) payload)
    {
      memory_double x = memory_positive_SNaNd ();
      uint64_t pl = (int64_t) payload;
      uint32_t pl_hi = (uint32_t) (pl >> 32);
      uint32_t pl_lo = (uint32_t) pl;
      x.word[DBL_EXPBIT0_WORD] =
        (x.word[DBL_EXPBIT0_WORD] & ~((1U << (DBL_MANT_DIG - 2 - 32)) - 1))
        | pl_hi;
      x.word[DBL_EXPBIT0_WORD + (DBL_EXPBIT0_WORD == 0 ? 1 : -1)] = pl_lo;
# if 0
      *result = x.value;
# else
      /* On 32-bit x86 processors, as well as on x86_64 processors with
         CC="gcc -mfpmath=387", the evaluation of *x and *y above is done
         through an 'fldl' instruction, which converts a signalling NaN to
         a quiet NaN. See
         <https://lists.gnu.org/archive/html/bug-gnulib/2023-10/msg00060.html>
         for details.  Use memcpy to avoid this.  */
      memcpy (result, &x.value, sizeof (double));
# endif
      return 0;
    }
  else
    {
      *result = 0.0;
      return -1;
    }
#else
# error "Please port gnulib setpayloadsig.c to your platform!"
#endif
}
