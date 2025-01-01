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

#include "fpe-trapping.h"
#include "macros.h"

/* musl libc does not support floating-point exception trapping, even where
   the hardware supports it.  See
   <https://wiki.musl-libc.org/functional-differences-from-glibc.html>  */
#if HAVE_FPE_TRAPPING && (!MUSL_LIBC || GNULIB_FEENABLEEXCEPT)

/* Test that feupdateenv(), unlike fesetenv(), can trigger traps.  */

int
main ()
{
  fenv_t env1;

  /* Get to a known initial state.  */
  ASSERT (feclearexcept (FE_ALL_EXCEPT) == 0);

  /* Enable trapping on FE_INVALID.  */
  if (sigfpe_on_invalid () < 0)
    {
      fputs ("Skipping test: trapping floating-point exceptions are not supported on this machine.\n", stderr);
      return 77;
    }

  /* Save the current environment in env1.  */
  ASSERT (fegetenv (&env1) == 0);

  /* Go back to the default environment.  */
  ASSERT (fesetenv (FE_DFL_ENV) == 0);

  /* Modify the current environment.  */
  ASSERT (feraiseexcept (FE_INVALID) == 0);

  /* Go back to env1.
     Since the exceptions flag FE_INVALID is currently set, and since
     env1 has trapping on FE_INVALID enabled, this should trap. */
  ASSERT (feupdateenv (&env1) == 0);

  return test_exit_status;
}

#else

int
main ()
{
  fputs ("Skipping test: feenableexcept not available\n", stderr);
  return 77;
}

#endif
