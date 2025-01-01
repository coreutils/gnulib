/* Construct a quiet NaN 'long double' with a given payload.
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

int
setpayloadl (long double *result, long double payload)
{
  return setpayload ((double *) result, payload);
}

#else

# include <float.h>
# include <stdint.h>

# include "signed-nan.h"
# include "snan.h"

/* 2^(LDBL_MANT_DIG-1).  */
# define TWO_LDBL_MANT_DIG \
    ((long double) (1U << ((LDBL_MANT_DIG - 1) / 4)) \
     * (long double) (1U << ((LDBL_MANT_DIG - 1 + 1) / 4)) \
     * (long double) (1U << ((LDBL_MANT_DIG - 1 + 2) / 4)) \
     * (long double) (1U << ((LDBL_MANT_DIG - 1 + 3) / 4)))

int
setpayloadl (long double *result, long double payload)
{
# if (LDBL_MANT_DIG == 64 || LDBL_MANT_DIG == 106 || LDBL_MANT_DIG == 113) \
     && defined LDBL_EXPBIT0_WORD && defined LDBL_EXPBIT0_BIT
  if (
#  if defined __hppa || (defined __mips__ && !MIPS_NAN2008_LONG_DOUBLE) || defined __sh__
      /* A zero payload is not allowed, because that would denote Infinity.
         Cf. snan.h.  */
      payload > 0.0L
#  else
      payload >= 0.0L
#  endif
#  if LDBL_MANT_DIG == 64 /* on i386, x86_64, ia64, m68k */
      && payload < 4611686018427387904.0L /* exp2l (LDBL_MANT_DIG - 2) */
      && payload == (long double) (int64_t) payload
#  endif
#  if LDBL_MANT_DIG == 106 /* on powerpc, powerpc64, powerpc64le */
      && payload < 2251799813685248.0L /* exp2l (DBL_MANT_DIG - 2) */
      && payload == (long double) (int64_t) payload
#  endif
#  if LDBL_MANT_DIG == 113 /* on alpha, arm64, loongarch64, mips64, riscv64, s390x, sparc64 */
      && payload < 2596148429267413814265248164610048.0L /* exp2l (LDBL_MANT_DIG - 2) */
      && payload == +(+(payload + TWO_LDBL_MANT_DIG) - TWO_LDBL_MANT_DIG)
#  endif
     )
    {
      memory_long_double x;
      x.value = positive_NaNl ();

#  if LDBL_MANT_DIG == 64 /* on i386, x86_64, ia64, m68k */
      uint64_t pl = (int64_t) payload;
#   if LDBL_EXPBIT0_WORD == 2 && LDBL_EXPBIT0_BIT == 0 /* on i386, x86_64, ia64 */
      x.word[1] = (x.word[1] & 0xC0000000U) | (uint32_t) (pl >> 32);
      x.word[0] = (uint32_t) pl;
#   elif LDBL_EXPBIT0_WORD == 0 && LDBL_EXPBIT0_BIT == 16 /* on m68k */
      x.word[1] = (x.word[1] & 0xC0000000U) | (uint32_t) (pl >> 32);
      x.word[2] = (uint32_t) pl;
#   else
#    error "Please port gnulib setpayloadl.c to your platform!"
#   endif
#  endif
#  if LDBL_MANT_DIG == 106 /* on powerpc, powerpc64, powerpc64le */
      uint64_t pl = (int64_t) payload;
#   if LDBL_EXPBIT0_BIT == 20
      uint32_t pl_hi = (uint32_t) (pl >> 32);
      uint32_t pl_lo = (uint32_t) pl;
      x.word[LDBL_EXPBIT0_WORD] = (x.word[LDBL_EXPBIT0_WORD] & 0xFFF80000U)
                                  | pl_hi;
      x.word[LDBL_EXPBIT0_WORD + (LDBL_EXPBIT0_WORD == 0 ? 1 : -1)] = pl_lo;
#   else
#    error "Please port gnulib setpayloadl.c to your platform!"
#   endif
#  endif
#  if LDBL_MANT_DIG == 113 /* on alpha, arm64, loongarch64, mips64, riscv64, s390x, sparc64 */
#   if LDBL_EXPBIT0_BIT == 16
      memory_long_double pl;
      pl.value = payload + TWO_LDBL_MANT_DIG;
      x.word[LDBL_EXPBIT0_WORD] = (x.word[LDBL_EXPBIT0_WORD] & 0xFFFF8000U)
                                  | (pl.word[LDBL_EXPBIT0_WORD] & 0x00007FFFU);
      x.word[LDBL_EXPBIT0_WORD + (LDBL_EXPBIT0_WORD == 0 ? 1 : -1)] =
        pl.word[LDBL_EXPBIT0_WORD + (LDBL_EXPBIT0_WORD == 0 ? 1 : -1)];
      x.word[LDBL_EXPBIT0_WORD + (LDBL_EXPBIT0_WORD == 0 ? 2 : -2)] =
        pl.word[LDBL_EXPBIT0_WORD + (LDBL_EXPBIT0_WORD == 0 ? 2 : -2)];
      x.word[LDBL_EXPBIT0_WORD + (LDBL_EXPBIT0_WORD == 0 ? 3 : -3)] =
        pl.word[LDBL_EXPBIT0_WORD + (LDBL_EXPBIT0_WORD == 0 ? 3 : -3)];
#   else
#    error "Please port gnulib setpayloadl.c to your platform!"
#   endif
#  endif

      *result = x.value;
      return 0;
    }
  else
    {
      *result = 0.0L;
      return -1;
    }
# else
#  error "Please port gnulib setpayloadl.c to your platform!"
# endif
}

#endif
