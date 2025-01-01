/* Test of saving the floating-point exception status flags.
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

int
main ()
{
  fexcept_t saved_flags_1;
  fexcept_t saved_flags_2;

  /* Test setting all exception flags.  */
  if (feraiseexcept (FE_INVALID | FE_DIVBYZERO | FE_OVERFLOW | FE_UNDERFLOW | FE_INEXACT) != 0)
    {
      fputs ("Skipping test: floating-point exceptions are not supported on this machine.\n", stderr);
      return 77;
    }

  /* Fill saved_flags_1.  */
  ASSERT (fegetexceptflag (&saved_flags_1,
                           FE_INVALID | FE_DIVBYZERO | FE_OVERFLOW | FE_UNDERFLOW | FE_INEXACT)
          == 0);

  /* Clear some of the exception flags.  */
  ASSERT (feclearexcept (FE_OVERFLOW | FE_UNDERFLOW | FE_INEXACT) == 0);
  /* Here, the set exception flags are FE_INVALID | FE_DIVBYZERO.  */
  ASSERT (fetestexcept (FE_INVALID) == FE_INVALID);
  ASSERT (fetestexcept (FE_DIVBYZERO) == FE_DIVBYZERO);
  ASSERT (fetestexcept (FE_OVERFLOW) == 0);
  ASSERT (fetestexcept (FE_UNDERFLOW) == 0);
  ASSERT (fetestexcept (FE_INEXACT) == 0);

  /* Fill saved_flags_2.  */
  ASSERT (fegetexceptflag (&saved_flags_2, FE_INVALID | FE_OVERFLOW) == 0);

  /* Restore some of the exception flags.  */
  ASSERT (fesetexceptflag (&saved_flags_1,
                           FE_INVALID | FE_DIVBYZERO | FE_OVERFLOW | FE_UNDERFLOW) == 0);
  /* Here, the set exception flags are FE_INVALID | FE_DIVBYZERO | FE_OVERFLOW | FE_UNDERFLOW.  */
  ASSERT (fetestexcept (FE_INVALID) == FE_INVALID);
  ASSERT (fetestexcept (FE_DIVBYZERO) == FE_DIVBYZERO);
  ASSERT (fetestexcept (FE_OVERFLOW) == FE_OVERFLOW);
  ASSERT (fetestexcept (FE_UNDERFLOW) == FE_UNDERFLOW);
  ASSERT (fetestexcept (FE_INEXACT) == 0);

  /* Clear some more exception flags.  */
  ASSERT (feclearexcept (FE_INVALID) == 0);
  /* Here, the set exception flags are FE_DIVBYZERO | FE_OVERFLOW | FE_UNDERFLOW.  */
  ASSERT (fetestexcept (FE_INVALID) == 0);
  ASSERT (fetestexcept (FE_DIVBYZERO) == FE_DIVBYZERO);
  ASSERT (fetestexcept (FE_OVERFLOW) == FE_OVERFLOW);
  ASSERT (fetestexcept (FE_UNDERFLOW) == FE_UNDERFLOW);
  ASSERT (fetestexcept (FE_INEXACT) == 0);

  /* Restore some more exception flags.  */
  ASSERT (fesetexceptflag (&saved_flags_2, FE_OVERFLOW) == 0);
  /* Here, the set exception flags are FE_DIVBYZERO | FE_UNDERFLOW.  */
  ASSERT (fetestexcept (FE_INVALID) == 0);
  ASSERT (fetestexcept (FE_DIVBYZERO) == FE_DIVBYZERO);
  ASSERT (fetestexcept (FE_OVERFLOW) == 0);
  ASSERT (fetestexcept (FE_UNDERFLOW) == FE_UNDERFLOW);
  ASSERT (fetestexcept (FE_INEXACT) == 0);

  /* Restore some more exception flags.  */
  ASSERT (fesetexceptflag (&saved_flags_2, FE_INVALID) == 0);
  /* Here, the set exception flags are FE_INVALID | FE_DIVBYZERO | FE_UNDERFLOW.  */
  ASSERT (fetestexcept (FE_INVALID) == FE_INVALID);
  ASSERT (fetestexcept (FE_DIVBYZERO) == FE_DIVBYZERO);
  ASSERT (fetestexcept (FE_OVERFLOW) == 0);
  ASSERT (fetestexcept (FE_UNDERFLOW) == FE_UNDERFLOW);
  ASSERT (fetestexcept (FE_INEXACT) == 0);

  /* ======================================================================== */
  /* Check that fesetexceptflag clears exception flags in both the 387 unit
     and the SSE unit, on i386 and x86_64 CPUs.  */

  fexcept_t saved_flags_3;

  ASSERT (feclearexcept (FE_INVALID) == 0);

  ASSERT (fegetexceptflag (&saved_flags_3, FE_INVALID) == 0);

  /* Set the FE_INVALID flag in the SSE unit.  */
  {
    static volatile double a, b;
    _GL_UNUSED volatile double c;
    a = 0.0; b = 0.0; c = a / b;
  }
  /* Set the FE_INVALID flag in the 387 unit.  */
  {
    static volatile long double al, bl;
    _GL_UNUSED volatile long double cl;
    al = 0.0L; bl = 0.0L; cl = al / bl;
  }

  /* Use fesetexceptflag to clear the FE_INVALID flag.  */
  ASSERT (fesetexceptflag (&saved_flags_3, FE_INVALID) == 0);

  /* Check that it's clear in both units.  */
  ASSERT (fetestexcept (FE_INVALID) == 0);

  return test_exit_status;
}
