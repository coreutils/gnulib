/* Extract the payload of a NaN 'long double'.
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

#if HAVE_SAME_LONG_DOUBLE_AS_DOUBLE

long double
getpayloadl (const long double *value)
{
  return getpayload ((const double *) value);
}

#else

# include <float.h>
# include <stdint.h>

# include "snan.h"

/* 2^(LDBL_MANT_DIG-1).  */
# define TWO_LDBL_MANT_DIG \
    ((long double) (1U << ((LDBL_MANT_DIG - 1) / 4)) \
     * (long double) (1U << ((LDBL_MANT_DIG - 1 + 1) / 4)) \
     * (long double) (1U << ((LDBL_MANT_DIG - 1 + 2) / 4)) \
     * (long double) (1U << ((LDBL_MANT_DIG - 1 + 3) / 4)))

long double
getpayloadl (const long double *value)
{
  if (isnanl (*value))
    {
# if (LDBL_MANT_DIG == 64 || LDBL_MANT_DIG == 106 || LDBL_MANT_DIG == 113) \
     && defined LDBL_EXPBIT0_WORD && defined LDBL_EXPBIT0_BIT
      memory_long_double x;
      x.value = *value;
#  if LDBL_MANT_DIG == 64 /* on i386, x86_64, ia64, m68k */
      int64_t payload;
#   if LDBL_EXPBIT0_WORD == 2 && LDBL_EXPBIT0_BIT == 0 /* on i386, x86_64, ia64 */
      payload = ((uint64_t) (x.word[1] & 0x3FFFFFFFU) << 32) | x.word[0];
#   elif LDBL_EXPBIT0_WORD == 0 && LDBL_EXPBIT0_BIT == 16 /* on m68k */
      payload = ((uint64_t) (x.word[1] & 0x3FFFFFFFU) << 32) | x.word[2];
#   else
#    error "Please port gnulib getpayloadl.c to your platform!"
#   endif
      return payload;
#  endif
#  if LDBL_MANT_DIG == 106 /* on powerpc, powerpc64, powerpc64le */
      int64_t payload;
#   if LDBL_EXPBIT0_BIT == 20
      payload = ((uint64_t) (x.word[LDBL_EXPBIT0_WORD] & 0x0007FFFFU) << 32)
                | x.word[LDBL_EXPBIT0_WORD + (LDBL_EXPBIT0_WORD == 0 ? 1 : -1)];
#   else
#    error "Please port gnulib getpayloadl.c to your platform!"
#   endif
      return payload;
#  endif
#  if LDBL_MANT_DIG == 113 /* on alpha, arm64, loongarch64, mips64, riscv64, s390x, sparc64 */
#   if LDBL_EXPBIT0_BIT == 16
      memory_long_double pl;
      pl.value = TWO_LDBL_MANT_DIG;
      pl.word[LDBL_EXPBIT0_WORD] |= x.word[LDBL_EXPBIT0_WORD] & 0x00007FFFU;
      pl.word[LDBL_EXPBIT0_WORD + (LDBL_EXPBIT0_WORD == 0 ? 1 : -1)] =
        x.word[LDBL_EXPBIT0_WORD + (LDBL_EXPBIT0_WORD == 0 ? 1 : -1)];
      pl.word[LDBL_EXPBIT0_WORD + (LDBL_EXPBIT0_WORD == 0 ? 2 : -2)] =
        x.word[LDBL_EXPBIT0_WORD + (LDBL_EXPBIT0_WORD == 0 ? 2 : -2)];
      pl.word[LDBL_EXPBIT0_WORD + (LDBL_EXPBIT0_WORD == 0 ? 3 : -3)] =
        x.word[LDBL_EXPBIT0_WORD + (LDBL_EXPBIT0_WORD == 0 ? 3 : -3)];
      return pl.value - TWO_LDBL_MANT_DIG;
#   else
#    error "Please port gnulib getpayloadl.c to your platform!"
#   endif
#  endif
# else
#  error "Please port gnulib getpayloadl.c to your platform!"
# endif
    }
  else
    return -1.0L;
}

#endif
