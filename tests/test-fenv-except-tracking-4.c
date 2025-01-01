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

/* Check that fesetexcept() works.  */

/* On *BSD/powerpc systems, raising FE_INVALID also sets FE_VXSOFT.  */
#ifndef FE_VXSOFT
# define FE_VXSOFT 0
#endif

int
main ()
{
  /* Test setting all exception flags.  */
  if (feraiseexcept (FE_ALL_EXCEPT) != 0)
    {
      fputs ("Skipping test: floating-point exceptions are not supported on this machine.\n", stderr);
      return 77;
    }

  /* Clear all exception flags.  */
  ASSERT (feclearexcept (FE_ALL_EXCEPT) == 0);


  /* Test setting just one exception flag: FE_INVALID.  */
  ASSERT (feclearexcept (FE_ALL_EXCEPT) == 0);
  ASSERT (fesetexcept (FE_INVALID) == 0);
  ASSERT ((fetestexcept (FE_ALL_EXCEPT) & ~FE_VXSOFT) == FE_INVALID);
  ASSERT (fetestexcept (FE_INVALID) == FE_INVALID);
  ASSERT (fetestexcept (FE_DIVBYZERO) == 0);
  ASSERT (fetestexcept (FE_OVERFLOW) == 0);
  ASSERT (fetestexcept (FE_UNDERFLOW) == 0);
  ASSERT (fetestexcept (FE_INEXACT) == 0);

  /* Test setting just one exception flag: FE_DIVBYZERO.  */
  ASSERT (feclearexcept (FE_ALL_EXCEPT) == 0);
  ASSERT (fesetexcept (FE_DIVBYZERO) == 0);
  ASSERT (fetestexcept (FE_ALL_EXCEPT) == FE_DIVBYZERO);
  ASSERT (fetestexcept (FE_INVALID) == 0);
  ASSERT (fetestexcept (FE_DIVBYZERO) == FE_DIVBYZERO);
  ASSERT (fetestexcept (FE_OVERFLOW) == 0);
  ASSERT (fetestexcept (FE_UNDERFLOW) == 0);
  ASSERT (fetestexcept (FE_INEXACT) == 0);

  /* Test setting just one exception flag: FE_OVERFLOW.  */
  ASSERT (feclearexcept (FE_ALL_EXCEPT) == 0);
  ASSERT (fesetexcept (FE_OVERFLOW) == 0);
  ASSERT (fetestexcept (FE_ALL_EXCEPT) == FE_OVERFLOW);
  ASSERT (fetestexcept (FE_INVALID) == 0);
  ASSERT (fetestexcept (FE_DIVBYZERO) == 0);
  ASSERT (fetestexcept (FE_OVERFLOW) == FE_OVERFLOW);
  ASSERT (fetestexcept (FE_UNDERFLOW) == 0);
  ASSERT (fetestexcept (FE_INEXACT) == 0);

  /* Test setting just one exception flag: FE_UNDERFLOW.  */
  ASSERT (feclearexcept (FE_ALL_EXCEPT) == 0);
  ASSERT (fesetexcept (FE_UNDERFLOW) == 0);
  ASSERT (fetestexcept (FE_ALL_EXCEPT) == FE_UNDERFLOW);
  ASSERT (fetestexcept (FE_INVALID) == 0);
  ASSERT (fetestexcept (FE_DIVBYZERO) == 0);
  ASSERT (fetestexcept (FE_OVERFLOW) == 0);
  ASSERT (fetestexcept (FE_UNDERFLOW) == FE_UNDERFLOW);
  ASSERT (fetestexcept (FE_INEXACT) == 0);

  /* Test setting just one exception flag: FE_INEXACT.  */
  ASSERT (feclearexcept (FE_ALL_EXCEPT) == 0);
  ASSERT (fesetexcept (FE_INEXACT) == 0);
  ASSERT (fetestexcept (FE_ALL_EXCEPT) == FE_INEXACT);
  ASSERT (fetestexcept (FE_INVALID) == 0);
  ASSERT (fetestexcept (FE_DIVBYZERO) == 0);
  ASSERT (fetestexcept (FE_OVERFLOW) == 0);
  ASSERT (fetestexcept (FE_UNDERFLOW) == 0);
  ASSERT (fetestexcept (FE_INEXACT) == FE_INEXACT);


  return test_exit_status;
}
