/* Extract the payload of a NaN 'double'.
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

double
getpayload (const double *value)
{
  if (isnand (*value))
    {
#if DBL_MANT_DIG == 53
      union { uint64_t i; double f; } x;
# if 0
      x.f = *value;
# else
      /* On 32-bit x86 processors, as well as on x86_64 processors with
         CC="gcc -mfpmath=387", the evaluation of *value above is done
         through an 'fldl' instruction, which converts a signalling NaN to
         a quiet NaN. See
         <https://lists.gnu.org/archive/html/bug-gnulib/2023-10/msg00060.html>
         for details.  Use memcpy to avoid this.  */
      memcpy (&x.f, value, sizeof (double));
# endif
      int64_t payload = x.i & (((uint64_t) 1 << (DBL_MANT_DIG - 2)) - 1);
      return payload;
#else
# error "Please port gnulib getpayload.c to your platform!"
#endif
    }
  else
    return -1.0;
}
