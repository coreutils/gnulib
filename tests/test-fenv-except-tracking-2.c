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

#if HAVE_FPE_TRAPPING

/* Check that triggering a floating-point operation can trigger a trap.  */

int
main (int argc, char *argv[])
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

  if (argc > 1)
    switch (argv[1][0])
      {
      case 'f':
        {
          volatile float a, b;
          _GL_UNUSED volatile float c;
          a = 0; b = 0; c = a / b;
        }
        break;

      case 'd':
        {
          volatile double a, b;
          _GL_UNUSED volatile double c;
          a = 0; b = 0; c = a / b;
        }
        break;

      case 'l':
        /* This test does not work on Linux/loongarch64 with glibc 2.37.
           Likewise on Linux/alpha with glibc 2.7 on Linux 2.6.26.
           Likewise on FreeBSD 12.2/sparc, NetBSD 8.0/sparc, OpenBSD 7.2/sparc64.
           Likewise on OpenBSD 7.4/mips64.
           Cause unknown.  */
        #if !((__GLIBC__ >= 2 && defined __loongarch__) \
              || ((__GLIBC__ == 2 && __GLIBC_MINOR__ < 36) && defined __alpha) \
              || ((defined __FreeBSD__ || defined __NetBSD__ || defined __OpenBSD__) && defined __sparc) \
              || (defined __OpenBSD__ && defined __mips64))
        {
          volatile long double a, b;
          _GL_UNUSED volatile long double c;
          a = 0; b = 0; c = a / b;
        }
        #else
        fputs ("Skipping test: known failure on this platform\n", stderr);
        return 77;
        #endif
        break;

      default:
        break;
      }

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
