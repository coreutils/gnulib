/* Total order of absolute value for 'double'.
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

#include <string.h>

#include "verify.h"

int
totalordermag (double const *x, double const *y)
{
  /* If one of *X, *Y is a NaN and the other isn't, the answer is easy:
     the NaN is "greater" than the other argument.  */
  int xn = isnand (*x);
  int yn = isnand (*y);
  if (!xn != !yn)
    return yn;
  /* If none of *X, *Y is a NaN, the '<=' operator on the absolute values
     does the job, including for -Infinity and +Infinity.  */
  if (!xn)
    return (signbit (*x) ? - *x : *x) <= (signbit (*y) ? - *y : *y);

  /* At this point, *X and *Y are NaNs.  */

#if defined DBL_SIGNBIT_WORD && defined DBL_SIGNBIT_BIT
  /* The use of a union to extract the bits of the representation of a
     'double' is safe in practice, despite of the "aliasing rules" of
     C99, because the GCC docs say
       "Even with '-fstrict-aliasing', type-punning is allowed, provided the
        memory is accessed through the union type."
     and similarly for other compilers.  */
# define NWORDS \
    ((sizeof (double) + sizeof (unsigned int) - 1) / sizeof (unsigned int))
  union { unsigned int word[NWORDS]; unsigned long long i; double value; }
    xu = {0}, yu = {0};

# if 0
  xu.value = *x;
  yu.value = *y;
# else
#  if defined __GNUC__ || defined __clang__
  /* Prevent gcc and clang from reusing the values of *x and *y (fetched above)
     in optimized inlined memcpy expansions.
     Seen with gcc <https://gcc.gnu.org/PR114659>
     and with clang 16.0.6 on OpenBSD 7.5.  */
  __asm__ __volatile__ ("" : : : "memory");
#  endif
  /* On 32-bit x86 processors, as well as on x86_64 processors with
     CC="gcc -mfpmath=387", the evaluation of *x and *y above is done through
     an 'fldl' instruction, which converts a signalling NaN to a quiet NaN. See
     <https://lists.gnu.org/archive/html/bug-gnulib/2023-10/msg00060.html>
     for details.  Use memcpy to avoid this.  */
  memcpy (&xu.value, x, sizeof (double));
  memcpy (&yu.value, y, sizeof (double));
# endif

  verify (NWORDS == 2);

  return ((xu.i & ~(1ULL << (DBL_SIGNBIT_BIT + 32))) /* *X without its sign bit */
# if defined __hppa || (defined __mips__ && !MIPS_NAN2008_DOUBLE) || defined __sh__
          /* Invert the most significant bit of the mantissa field.
             Cf. snan.h.  */
          ^ (1ULL << 51)
# endif
         ) <=
         ((yu.i & ~(1ULL << (DBL_SIGNBIT_BIT + 32))) /* *Y without its sign bit */
# if defined __hppa || (defined __mips__ && !MIPS_NAN2008_DOUBLE) || defined __sh__
          /* Invert the most significant bit of the mantissa field.
             Cf. snan.h.  */
          ^ (1ULL << 51)
# endif
         );
#else
# error "Please port gnulib totalordermagf.c to your platform!"
#endif
}
