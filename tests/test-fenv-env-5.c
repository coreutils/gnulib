/* Test of controlling the floating-point environment.
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

/* Test the combination of feholdexcept() with feupdateenv().  */

/* Some CPUs don't support FE_UPWARD and FE_DOWNWARD in hardware.  */
#if defined __sh__
# ifndef FE_UPWARD
#  define FE_UPWARD   FE_TOWARDZERO
#  define FE_DOWNWARD FE_TONEAREST
# endif
#endif

/* On *BSD/powerpc systems, raising FE_INVALID also sets FE_VXSOFT.  */
#ifndef FE_VXSOFT
# define FE_VXSOFT 0
#endif

int
main ()
{
#if defined __GLIBC__ && defined __arm__ && defined __SOFTFP__
  fputs ("Skipping test: no floating-point environment exists on this machine\n", stderr);
  return 77;
#else
  fenv_t env1, env2;

  /* Get to a known initial state.  */
  ASSERT (feclearexcept (FE_ALL_EXCEPT) == 0);

  /* Save the current environment in env1.  */
  ASSERT (feholdexcept (&env1) == 0);

  /* Modify the current environment.  */
  fesetround (FE_UPWARD);
  int supports_tracking = (feraiseexcept (FE_INVALID | FE_OVERFLOW | FE_INEXACT) == 0);
  int supports_trapping = (feenableexcept (FE_DIVBYZERO) != -1);

  /* Save the current environment in env2.  */
  ASSERT (feholdexcept (&env2) == 0);

  /* Check that the exception flags are cleared.  */
  ASSERT (fetestexcept (FE_ALL_EXCEPT) == 0);
  /* Check that the exception trap bits are cleared.  */
  ASSERT (fegetexcept () == 0);

  /* Go back to env1.  */
  ASSERT (feupdateenv (&env1) == 0);

  /* Check that the rounding direction has been restored.  */
  ASSERT (fegetround () == FE_TONEAREST);
  /* Check that the exception flags are the union of the saved and of the
     current exception flags.  (The saved exception flags and the current
     exception flags both happen to be none in this case.)  */
  ASSERT (fetestexcept (FE_ALL_EXCEPT) == 0);
  /* Check that the exception trap bits have been restored.  */
  ASSERT (fegetexcept () == 0);

  /* Modify the rounding direction, the exception flags, and the exception
     trap bits again.  */
  fesetround (FE_DOWNWARD);
  ASSERT (fegetround () == FE_DOWNWARD);
  feclearexcept (FE_OVERFLOW);
  feraiseexcept (FE_UNDERFLOW | FE_INEXACT);
  ASSERT (fetestexcept (FE_ALL_EXCEPT) == (supports_tracking ? FE_UNDERFLOW | FE_INEXACT : 0));
  feenableexcept (FE_INVALID);
  ASSERT (fegetexcept () == (supports_trapping ? FE_INVALID : 0));

  /* Go back to env2.  */
  ASSERT (feupdateenv (&env2) == 0);

  /* Check that the rounding direction has been restored.  */
  ASSERT (fegetround () == FE_UPWARD);
  /* Check that the exception flags are the union of the saved and of the
     current exception flags.  */
  if (supports_tracking)
    ASSERT ((fetestexcept (FE_ALL_EXCEPT) & ~FE_VXSOFT) == (FE_INVALID | FE_OVERFLOW | FE_UNDERFLOW | FE_INEXACT));
  else
    ASSERT (fetestexcept (FE_ALL_EXCEPT) == 0);
  /* Check that the exception trap bits have been restored.  */
  ASSERT (fegetexcept () == (supports_trapping ? FE_DIVBYZERO : 0));

  /* ======================================================================== */
  /* FE_DFL_ENV */

  /* Go back to the default environment.  */
  ASSERT (feupdateenv (FE_DFL_ENV) == 0);

  /* Check that the rounding direction has been restored,
     whereas the exception flags are unmodified.  */
  ASSERT (fegetround () == FE_TONEAREST);
  if (supports_tracking)
    ASSERT ((fetestexcept (FE_ALL_EXCEPT) & ~FE_VXSOFT) == (FE_INVALID | FE_OVERFLOW | FE_UNDERFLOW | FE_INEXACT));
  else
    ASSERT (fetestexcept (FE_ALL_EXCEPT) == 0);

  /* Enable trapping on FE_INVALID.  */
  feclearexcept (FE_INVALID);
  feenableexcept (FE_INVALID);
  ASSERT (fetestexcept (FE_ALL_EXCEPT) == (supports_tracking ? FE_OVERFLOW | FE_UNDERFLOW | FE_INEXACT : 0));

  /* Go back to the default environment.  */
  ASSERT (feupdateenv (FE_DFL_ENV) == 0);

  /* Check that the rounding direction has been restored,
     whereas the exception flags are unmodified.  */
  ASSERT (fegetround () == FE_TONEAREST);
  ASSERT (fetestexcept (FE_ALL_EXCEPT) == (supports_tracking ? FE_OVERFLOW | FE_UNDERFLOW | FE_INEXACT : 0));

  /* Check that it has trapping on FE_INVALID disabled.  */
  ASSERT (fegetexcept () == 0);
  {
    double volatile a;
    _GL_UNUSED double volatile b;
    a = 0; b = a / a;
  }

  /* ======================================================================== */
  /* Check that feholdexcept, unlike fegetenv, disables trapping.  */

  /* musl libc does not support floating-point exception trapping, even where
     the hardware supports it.  See
     <https://wiki.musl-libc.org/functional-differences-from-glibc.html>  */
# if !MUSL_LIBC || GNULIB_FEENABLEEXCEPT
  /* Enable trapping on FE_INVALID.  */
  feclearexcept (FE_INVALID);
  if (feenableexcept (FE_INVALID) != -1)
    {
      /* Call feholdexcept.  */
      ASSERT (feholdexcept (&env1) == 0);

      /* Check that it has disabled trapping on FE_INVALID.  */
      ASSERT (fegetexcept () == 0);
      {
        double volatile a;
        _GL_UNUSED double volatile b;
        a = 0; b = a / a;
      }
    }
# endif

  return test_exit_status;
#endif
}
