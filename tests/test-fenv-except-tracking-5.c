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

#include <stdio.h>

#include "fpe-trapping.h"
#include "macros.h"

/* musl libc does not support floating-point exception trapping, even where
   the hardware supports it.  See
   <https://wiki.musl-libc.org/functional-differences-from-glibc.html>  */
#if HAVE_FPE_TRAPPING && (!MUSL_LIBC || GNULIB_FEENABLEEXCEPT)

/* Check that fesetexcept() does not trigger a trap.  */

static volatile double a, b;
static volatile long double al, bl;

int
main ()
{
  /* Clear FE_INVALID exceptions from past operations.  */
  feclearexcept (FE_INVALID);

  /* An FE_INVALID exception shall trigger a SIGFPE signal, which by default
     terminates the program.  */
  if (sigfpe_on_invalid () < 0)
    {
      fputs ("Skipping test: trapping floating-point exceptions are not supported on this machine.\n", stderr);
      return 77;
    }

  /* Attempt to set the FE_INVALID exception flag.  */
  _GL_UNUSED int rc = fesetexcept (FE_INVALID);
  /* On older i386 and on PowerPC, there is no way to implement
     fesetexcept() such that it does not trigger a trap.  fesetexcept()
     is expected to fail in this case.  */
# if !((defined __i386 || defined _M_IX86) || defined _ARCH_PPC)
  ASSERT (rc == 0);
# endif

  /* Do a harmless floating-point operation (since on some CPUs, floating-point
     exceptions trigger a trap only at the next floating-point operation).  */
  a = 1.0; b = a + a;
  al = 1.0L; bl = al + al;

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
