/* Total order of absolute value for 'long double'.
   Copyright 2023-2025 Free Software Foundation, Inc.

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

/* Written by Paul Eggert and Bruno Haible.  */

#include <config.h>

/* Specification.  */
#include <math.h>

#if HAVE_SAME_LONG_DOUBLE_AS_DOUBLE

int
totalordermagl (long double const *x, long double const *y)
{
  return totalordermag ((double const *) x, (double const *) y);
}

#else

# include <float.h>
# include <stdint.h>
# include <string.h>

# include "verify.h"

int
totalordermagl (long double const *x, long double const *y)
{
  /* If one of *X, *Y is a NaN and the other isn't, the answer is easy:
     the NaN is "greater" than the other argument.  */
  int xn = isnanl (*x);
  int yn = isnanl (*y);
  if (!xn != !yn)
    return yn;
  /* If none of *X, *Y is a NaN, the '<=' operator on the absolute values
     does the job, including for -Infinity and +Infinity.  */
  if (!xn)
    return (signbit (*x) ? - *x : *x) <= (signbit (*y) ? - *y : *y);

  /* At this point, *X and *Y are NaNs.  */

# if defined LDBL_SIGNBIT_WORD && defined LDBL_SIGNBIT_BIT
  /* The use of a union to extract the bits of the representation of a
     'double' is safe in practice, despite of the "aliasing rules" of
     C99, because the GCC docs say
       "Even with '-fstrict-aliasing', type-punning is allowed, provided the
        memory is accessed through the union type."
     and similarly for other compilers.  */
#  define NWORDS \
    ((sizeof (long double) + sizeof (unsigned int) - 1) / sizeof (unsigned int))
#  if LDBL_MANT_DIG == 64 /* on i386, x86_64, ia64, m68k */
  /* A single uint64_t is enough to hold the payload, since we ignore the sign
     bit and the exponent has the maximum possible value.  */
  verify (NWORDS >= 3);
#   if LDBL_SIGNBIT_WORD == 2 && LDBL_SIGNBIT_BIT == 15 /* on i386, x86_64, ia64 */
  /* Also, LDBL_EXPBIT0_WORD == 2 && LDBL_EXPBIT0_BIT == 0.  */
  union { unsigned int word[NWORDS]; uint64_t i; long double value; } xu, yu;
  xu.value = *x;
  yu.value = *y;
  return xu.i <= yu.i;
#   elif LDBL_SIGNBIT_WORD == 0 && LDBL_SIGNBIT_BIT == 31 /* on m68k */
  /* Also, LDBL_EXPBIT0_WORD == 0 && LDBL_EXPBIT0_BIT == 16.  */
  union { unsigned int word[NWORDS]; long double value; } xu, yu;
  xu.value = *x;
  yu.value = *y;
  /* The payload is in word[1] (high part) and word[2] (low part).  */
  return (xu.word[1] < yu.word[1])
         | ((xu.word[1] == yu.word[1]) & (xu.word[2] <= yu.word[2]));
#   else
#    error "Please port gnulib totalordermagl.c to your platform!"
#   endif
#  elif LDBL_MANT_DIG == 106 /* on powerpc, powerpc64, powerpc64le */
  /* Two uint64_t are needed to hold the payload.
     In the double-double representation, each of the two uint64_t holds
     a sign bit.  */
  verify (NWORDS == 4);
  union { unsigned int word[NWORDS]; uint64_t i[2]; long double value; }
    xu = {0}, yu = {0};
  xu.value = *x;
  yu.value = *y;
  uint64_t xhi = xu.i[0] & ~((uint64_t) 1 << (LDBL_SIGNBIT_BIT + 32));
  uint64_t xlo = xu.i[1] & ~((uint64_t) 1 << (LDBL_SIGNBIT_BIT + 32));
  uint64_t yhi = yu.i[0] & ~((uint64_t) 1 << (LDBL_SIGNBIT_BIT + 32));
  uint64_t ylo = yu.i[1] & ~((uint64_t) 1 << (LDBL_SIGNBIT_BIT + 32));
  return (xhi < yhi) | ((xhi == yhi) & (xlo <= ylo));
#  else
  /* Here, LDBL_MANT_DIG == 113 (alpha, arm64, loongarch64, mips64, riscv64,
                                 s390x, sparc64).  */
  /* Two uint64_t are needed to hold the payload.  */
  verify (NWORDS == 4);
  union { unsigned int word[NWORDS]; uint64_t i[2]; long double value; }
    xu = {0}, yu = {0};
# if 0
  xu.value = *x;
  yu.value = *y;
# else
  /* On Linux/SPARC in 64-bit mode, gcc-6.4.0 -O2 miscompiles the simple
     assignments above.  Use memcpy as a workaround.  */
  memcpy (&xu.value, x, sizeof (long double));
  memcpy (&yu.value, y, sizeof (long double));
# endif
  uint64_t xhi;
  uint64_t xlo;
  uint64_t yhi;
  uint64_t ylo;
#   if LDBL_SIGNBIT_WORD < 2 /* big-endian */
  xhi = xu.i[0] & ~((uint64_t) 1 << (LDBL_SIGNBIT_BIT + 32 * (1 - LDBL_SIGNBIT_WORD)));
  xlo = xu.i[1];
  yhi = yu.i[0] & ~((uint64_t) 1 << (LDBL_SIGNBIT_BIT + 32 * (1 - LDBL_SIGNBIT_WORD)));
  ylo = yu.i[1];
#   else /* little-endian */
  xhi = xu.i[1] & ~((uint64_t) 1 << (LDBL_SIGNBIT_BIT + 32 * (LDBL_SIGNBIT_WORD - 2)));
  xlo = xu.i[0];
  yhi = yu.i[1] & ~((uint64_t) 1 << (LDBL_SIGNBIT_BIT + 32 * (LDBL_SIGNBIT_WORD - 2)));
  ylo = yu.i[0];
#   endif
#   if defined __hppa || (defined __mips__ && !MIPS_NAN2008_LONG_DOUBLE) || defined __sh__
  /* Invert the most significant bit of the mantissa field.  Cf. snan.h.  */
  xhi ^=
    (1ULL << (LDBL_MANT_DIG == 106
              ? 51                          /* double-double representation */
              : (LDBL_MANT_DIG - 2) - 64)); /* quad precision representation */
  yhi ^=
    (1ULL << (LDBL_MANT_DIG == 106
              ? 51                          /* double-double representation */
              : (LDBL_MANT_DIG - 2) - 64)); /* quad precision representation */
#   endif
  return (xhi < yhi) | ((xhi == yhi) & (xlo <= ylo));
#  endif
# else
#  error "Please port gnulib totalordermagl.c to your platform!"
# endif
}

#endif
