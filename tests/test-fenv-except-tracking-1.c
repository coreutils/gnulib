/* Test of tracking of floating-point exceptions.
   Copyright (C) 2023-2025 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Written by Bruno Haible <bruno@clisp.org>, 2023.  */

#include <config.h>

/* Specification.  */
#include <fenv.h>

#include "macros.h"

/* On *BSD/powerpc systems, raising FE_INVALID also sets FE_VXSOFT.  */
#ifndef FE_VXSOFT
# define FE_VXSOFT 0
#endif
#ifndef FE_VXZDZ
# define FE_VXZDZ 0
#endif

static volatile double a, b, c;

int
main ()
{
  /* Test setting all exception flags.  */
  if (feraiseexcept (FE_ALL_EXCEPT) != 0)
    {
      fputs ("Skipping test: floating-point exceptions are not supported on this machine.\n", stderr);
      return 77;
    }
  ASSERT (/* with the libc's feraiseexcept(): */
          fetestexcept (FE_ALL_EXCEPT) == FE_ALL_EXCEPT
          || /* with gnulib's feraiseexcept(): */
             (fetestexcept (FE_ALL_EXCEPT) & ~FE_VXSOFT)
             == (FE_DIVBYZERO | FE_INEXACT | FE_INVALID | FE_OVERFLOW | FE_UNDERFLOW));
  ASSERT (fetestexcept (FE_INVALID) == FE_INVALID);
  ASSERT (fetestexcept (FE_DIVBYZERO) == FE_DIVBYZERO);
  ASSERT (fetestexcept (FE_OVERFLOW) == FE_OVERFLOW);
  ASSERT (fetestexcept (FE_UNDERFLOW) == FE_UNDERFLOW);
  ASSERT (fetestexcept (FE_INEXACT) == FE_INEXACT);

  /* Test clearing all exception flags.  */
  ASSERT (feclearexcept (FE_ALL_EXCEPT) == 0);
  ASSERT (fetestexcept (FE_ALL_EXCEPT) == 0);
  ASSERT (fetestexcept (FE_INVALID) == 0);
  ASSERT (fetestexcept (FE_DIVBYZERO) == 0);
  ASSERT (fetestexcept (FE_OVERFLOW) == 0);
  ASSERT (fetestexcept (FE_UNDERFLOW) == 0);
  ASSERT (fetestexcept (FE_INEXACT) == 0);


  /* Test setting just one exception flag: FE_INVALID.  */
  ASSERT (feclearexcept (FE_ALL_EXCEPT) == 0);
  ASSERT (feraiseexcept (FE_INVALID) == 0);
  ASSERT ((fetestexcept (FE_ALL_EXCEPT) & ~FE_VXSOFT) == FE_INVALID);
  ASSERT (fetestexcept (FE_INVALID) == FE_INVALID);
  ASSERT (fetestexcept (FE_DIVBYZERO) == 0);
  ASSERT (fetestexcept (FE_OVERFLOW) == 0);
  ASSERT (fetestexcept (FE_UNDERFLOW) == 0);
  ASSERT (fetestexcept (FE_INEXACT) == 0);

  /* Test setting just one exception flag: FE_DIVBYZERO.  */
  ASSERT (feclearexcept (FE_ALL_EXCEPT) == 0);
  ASSERT (feraiseexcept (FE_DIVBYZERO) == 0);
  ASSERT (fetestexcept (FE_ALL_EXCEPT) == FE_DIVBYZERO);
  ASSERT (fetestexcept (FE_INVALID) == 0);
  ASSERT (fetestexcept (FE_DIVBYZERO) == FE_DIVBYZERO);
  ASSERT (fetestexcept (FE_OVERFLOW) == 0);
  ASSERT (fetestexcept (FE_UNDERFLOW) == 0);
  ASSERT (fetestexcept (FE_INEXACT) == 0);

  /* Test setting just one exception flag: FE_OVERFLOW.
     On many architectures, this has the side-effect of also setting FE_INEXACT:
     arm64, arm, alpha, hppa, ia64, loongarch64, s390, sh, sparc.  */
  ASSERT (feclearexcept (FE_ALL_EXCEPT) == 0);
  ASSERT (feraiseexcept (FE_OVERFLOW) == 0);
  ASSERT ((fetestexcept (FE_ALL_EXCEPT) & ~FE_INEXACT) == FE_OVERFLOW);
  ASSERT (fetestexcept (FE_INVALID) == 0);
  ASSERT (fetestexcept (FE_DIVBYZERO) == 0);
  ASSERT (fetestexcept (FE_OVERFLOW) == FE_OVERFLOW);
  ASSERT (fetestexcept (FE_UNDERFLOW) == 0);

  /* Test setting just one exception flag: FE_UNDERFLOW.
     On many architectures, this has the side-effect of also setting FE_INEXACT:
     arm64, arm, alpha, hppa, ia64, loongarch64, s390, sh, sparc.  */
  ASSERT (feclearexcept (FE_ALL_EXCEPT) == 0);
  ASSERT (feraiseexcept (FE_UNDERFLOW) == 0);
  ASSERT ((fetestexcept (FE_ALL_EXCEPT) & ~FE_INEXACT) == FE_UNDERFLOW);
  ASSERT (fetestexcept (FE_INVALID) == 0);
  ASSERT (fetestexcept (FE_DIVBYZERO) == 0);
  ASSERT (fetestexcept (FE_OVERFLOW) == 0);
  ASSERT (fetestexcept (FE_UNDERFLOW) == FE_UNDERFLOW);

  /* Test setting just one exception flag: FE_INEXACT.  */
  ASSERT (feclearexcept (FE_ALL_EXCEPT) == 0);
  ASSERT (feraiseexcept (FE_INEXACT) == 0);
  ASSERT (fetestexcept (FE_ALL_EXCEPT) == FE_INEXACT);
  ASSERT (fetestexcept (FE_INVALID) == 0);
  ASSERT (fetestexcept (FE_DIVBYZERO) == 0);
  ASSERT (fetestexcept (FE_OVERFLOW) == 0);
  ASSERT (fetestexcept (FE_UNDERFLOW) == 0);
  ASSERT (fetestexcept (FE_INEXACT) == FE_INEXACT);


  /* Test clearing just one exception flag: FE_INVALID.  */
  ASSERT (feraiseexcept (FE_ALL_EXCEPT) == 0);
  ASSERT (feclearexcept (FE_INVALID) == 0);
  ASSERT (/* with the libc's feraiseexcept(): */
          fetestexcept (FE_ALL_EXCEPT) == (FE_ALL_EXCEPT & ~FE_INVALID)
          || /* with gnulib's feraiseexcept(): */
             fetestexcept (FE_ALL_EXCEPT)
             == (FE_DIVBYZERO | FE_INEXACT | FE_OVERFLOW | FE_UNDERFLOW));
  ASSERT (fetestexcept (FE_INVALID) == 0);
  ASSERT (fetestexcept (FE_DIVBYZERO) == FE_DIVBYZERO);
  ASSERT (fetestexcept (FE_OVERFLOW) == FE_OVERFLOW);
  ASSERT (fetestexcept (FE_UNDERFLOW) == FE_UNDERFLOW);
  ASSERT (fetestexcept (FE_INEXACT) == FE_INEXACT);

  /* Test clearing just one exception flag: FE_DIVBYZERO.  */
  ASSERT (feraiseexcept (FE_ALL_EXCEPT) == 0);
  ASSERT (feclearexcept (FE_DIVBYZERO) == 0);
  ASSERT (/* with the libc's feraiseexcept(): */
          fetestexcept (FE_ALL_EXCEPT) == (FE_ALL_EXCEPT & ~FE_DIVBYZERO)
          || /* with gnulib's feraiseexcept(): */
             (fetestexcept (FE_ALL_EXCEPT) & ~FE_VXSOFT)
             == (FE_INEXACT | FE_INVALID | FE_OVERFLOW | FE_UNDERFLOW));
  ASSERT (fetestexcept (FE_INVALID) == FE_INVALID);
  ASSERT (fetestexcept (FE_DIVBYZERO) == 0);
  ASSERT (fetestexcept (FE_OVERFLOW) == FE_OVERFLOW);
  ASSERT (fetestexcept (FE_UNDERFLOW) == FE_UNDERFLOW);
  ASSERT (fetestexcept (FE_INEXACT) == FE_INEXACT);

  /* Test clearing just one exception flag: FE_OVERFLOW.  */
  ASSERT (feraiseexcept (FE_ALL_EXCEPT) == 0);
  ASSERT (feclearexcept (FE_OVERFLOW) == 0);
  ASSERT (/* with the libc's feraiseexcept(): */
          fetestexcept (FE_ALL_EXCEPT) == (FE_ALL_EXCEPT & ~FE_OVERFLOW)
          || /* with gnulib's feraiseexcept(): */
             (fetestexcept (FE_ALL_EXCEPT) & ~FE_VXSOFT)
             == (FE_DIVBYZERO | FE_INEXACT | FE_INVALID | FE_UNDERFLOW));
  ASSERT (fetestexcept (FE_INVALID) == FE_INVALID);
  ASSERT (fetestexcept (FE_DIVBYZERO) == FE_DIVBYZERO);
  ASSERT (fetestexcept (FE_OVERFLOW) == 0);
  ASSERT (fetestexcept (FE_UNDERFLOW) == FE_UNDERFLOW);
  ASSERT (fetestexcept (FE_INEXACT) == FE_INEXACT);

  /* Test clearing just one exception flag: FE_UNDERFLOW.  */
  ASSERT (feraiseexcept (FE_ALL_EXCEPT) == 0);
  ASSERT (feclearexcept (FE_UNDERFLOW) == 0);
  ASSERT (/* with the libc's feraiseexcept(): */
          fetestexcept (FE_ALL_EXCEPT) == (FE_ALL_EXCEPT & ~FE_UNDERFLOW)
          || /* with gnulib's feraiseexcept(): */
             (fetestexcept (FE_ALL_EXCEPT) & ~FE_VXSOFT)
             == (FE_DIVBYZERO | FE_INEXACT | FE_INVALID | FE_OVERFLOW));
  ASSERT (fetestexcept (FE_INVALID) == FE_INVALID);
  ASSERT (fetestexcept (FE_DIVBYZERO) == FE_DIVBYZERO);
  ASSERT (fetestexcept (FE_OVERFLOW) == FE_OVERFLOW);
  ASSERT (fetestexcept (FE_UNDERFLOW) == 0);
  ASSERT (fetestexcept (FE_INEXACT) == FE_INEXACT);

  /* Test clearing just one exception flag: FE_INEXACT.  */
  ASSERT (feraiseexcept (FE_ALL_EXCEPT) == 0);
  ASSERT (feclearexcept (FE_INEXACT) == 0);
  ASSERT (/* with the libc's feraiseexcept(): */
          fetestexcept (FE_ALL_EXCEPT) == (FE_ALL_EXCEPT & ~FE_INEXACT)
          || /* with gnulib's feraiseexcept(): */
             (fetestexcept (FE_ALL_EXCEPT) & ~FE_VXSOFT)
             == (FE_DIVBYZERO | FE_INVALID | FE_OVERFLOW | FE_UNDERFLOW));
  ASSERT (fetestexcept (FE_INVALID) == FE_INVALID);
  ASSERT (fetestexcept (FE_DIVBYZERO) == FE_DIVBYZERO);
  ASSERT (fetestexcept (FE_OVERFLOW) == FE_OVERFLOW);
  ASSERT (fetestexcept (FE_UNDERFLOW) == FE_UNDERFLOW);
  ASSERT (fetestexcept (FE_INEXACT) == 0);


#if !(defined __arm__ && defined __SOFTFP__)
  /* Test the effects of an operation that produces FE_INVALID.  */
  ASSERT (feclearexcept (FE_ALL_EXCEPT) == 0);
  a = 0; b = 0; c = a / b;
  ASSERT ((fetestexcept (FE_ALL_EXCEPT) & ~FE_VXZDZ) == FE_INVALID);
  ASSERT (fetestexcept (FE_INVALID) == FE_INVALID);
  ASSERT (fetestexcept (FE_DIVBYZERO) == 0);
  ASSERT (fetestexcept (FE_OVERFLOW) == 0);
  ASSERT (fetestexcept (FE_UNDERFLOW) == 0);
  ASSERT (fetestexcept (FE_INEXACT) == 0);

  /* Test the effects of an operation that produces FE_DIVBYZERO.  */
  ASSERT (feclearexcept (FE_ALL_EXCEPT) == 0);
  a = 3; b = 0; c = a / b;
  ASSERT (fetestexcept (FE_ALL_EXCEPT) == FE_DIVBYZERO);
  ASSERT (fetestexcept (FE_INVALID) == 0);
  ASSERT (fetestexcept (FE_DIVBYZERO) == FE_DIVBYZERO);
  ASSERT (fetestexcept (FE_OVERFLOW) == 0);
  ASSERT (fetestexcept (FE_UNDERFLOW) == 0);
  ASSERT (fetestexcept (FE_INEXACT) == 0);

  /* Test the effects of an operation that produces FE_OVERFLOW.  */
  ASSERT (feclearexcept (FE_ALL_EXCEPT) == 0);
  a = 1; for (int i = 0; i < 800; i++) a = 2.0 * a;
  b = 1; for (int i = 0; i < 700; i++) b = 2.0 * b;
  c = a * b;
  {
    int exc = fetestexcept (FE_ALL_EXCEPT);
    ASSERT ((FE_OVERFLOW & ~exc) == 0);
    ASSERT ((exc & ~(FE_OVERFLOW | FE_INEXACT)) == 0);
  }
  ASSERT (fetestexcept (FE_INVALID) == 0);
  ASSERT (fetestexcept (FE_DIVBYZERO) == 0);
  ASSERT (fetestexcept (FE_OVERFLOW) == FE_OVERFLOW);
  ASSERT (fetestexcept (FE_UNDERFLOW) == 0);

  /* Test the effects of an operation that produces FE_UNDERFLOW.  */
  ASSERT (feclearexcept (FE_ALL_EXCEPT) == 0);
  a = 1; for (int i = 0; i < 800; i++) a = 0.5 * a;
  b = 1; for (int i = 0; i < 700; i++) b = 0.5 * b;
  c = a * b;
  {
    int exc = fetestexcept (FE_ALL_EXCEPT);
    ASSERT ((FE_UNDERFLOW & ~exc) == 0);
    ASSERT ((exc & ~(FE_UNDERFLOW | FE_INEXACT)) == 0);
  }
  ASSERT (fetestexcept (FE_INVALID) == 0);
  ASSERT (fetestexcept (FE_DIVBYZERO) == 0);
  ASSERT (fetestexcept (FE_OVERFLOW) == 0);
  ASSERT (fetestexcept (FE_UNDERFLOW) == FE_UNDERFLOW);

/* On alpha, this test works only when compiled with the GCC option
   '-mieee-with-inexact'.  */
# if !defined __alpha
  /* Test the effects of an operation that produces FE_INEXACT.  */
  ASSERT (feclearexcept (FE_ALL_EXCEPT) == 0);
  a = 1; b = 3; c = a / b;
  ASSERT (fetestexcept (FE_ALL_EXCEPT) == FE_INEXACT);
  ASSERT (fetestexcept (FE_INVALID) == 0);
  ASSERT (fetestexcept (FE_DIVBYZERO) == 0);
  ASSERT (fetestexcept (FE_OVERFLOW) == 0);
  ASSERT (fetestexcept (FE_UNDERFLOW) == 0);
  ASSERT (fetestexcept (FE_INEXACT) == FE_INEXACT);
# endif
#endif


  return test_exit_status;
}
