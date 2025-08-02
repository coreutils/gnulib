/* Total order of absolute value for 'float'.
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
totalordermagf (float const *x, float const *y)
{
  /* If one of *X, *Y is a NaN and the other isn't, the answer is easy:
     the NaN is "greater" than the other argument.  */
  int xn = isnanf (*x);
  int yn = isnanf (*y);
  if (!xn != !yn)
    return yn;
  /* If none of *X, *Y is a NaN, the '<=' operator on the absolute values
     does the job, including for -Infinity and +Infinity.  */
  if (!xn)
    return (signbit (*x) ? - *x : *x) <= (signbit (*y) ? - *y : *y);

  /* At this point, *X and *Y are NaNs.  */

#if defined FLT_SIGNBIT_WORD && defined FLT_SIGNBIT_BIT
  /* The use of a union to extract the bits of the representation of a
     'float' is safe in practice, despite of the "aliasing rules" of
     C99, because the GCC docs say
       "Even with '-fstrict-aliasing', type-punning is allowed, provided the
        memory is accessed through the union type."
     and similarly for other compilers.  */
# define NWORDS \
    ((sizeof (float) + sizeof (unsigned int) - 1) / sizeof (unsigned int))
  union { unsigned int word[NWORDS]; float value; }
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
     an 'flds' instruction, which converts a signalling NaN to a quiet NaN. See
     <https://lists.gnu.org/archive/html/bug-gnulib/2023-10/msg00060.html>
     for details.  Use memcpy to avoid this.  */
  memcpy (&xu.value, x, sizeof (float));
  memcpy (&yu.value, y, sizeof (float));
# endif

  verify (NWORDS == 1);

  return ((xu.word[0] & ~(1U << FLT_SIGNBIT_BIT)) /* *X without its sign bit */
# if defined __hppa || (defined __mips__ && !MIPS_NAN2008_FLOAT) || defined __sh__
          /* Invert the most significant bit of the mantissa field.
             Cf. snan.h.  */
          ^ (1U << 22)
# endif
         ) <=
         ((yu.word[0] & ~(1U << FLT_SIGNBIT_BIT)) /* *Y without its sign bit */
# if defined __hppa || (defined __mips__ && !MIPS_NAN2008_FLOAT) || defined __sh__
          /* Invert the most significant bit of the mantissa field.
             Cf. snan.h.  */
          ^ (1U << 22)
# endif
         );
#else
# error "Please port gnulib totalordermagf.c to your platform!"
#endif
}
