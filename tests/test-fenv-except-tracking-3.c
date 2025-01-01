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
#include <stdlib.h>
#include <string.h>

#include "macros.h"

/* musl libc does not support floating-point exception trapping, even where
   the hardware supports it.  See
   <https://wiki.musl-libc.org/functional-differences-from-glibc.html>  */
#if !MUSL_LIBC || GNULIB_FEENABLEEXCEPT

/* Check that feraiseexcept() can trigger a trap.  */

int
main (int argc, char *argv[])
{
  if (argc > 1)
    {
      int exception;

      if (STREQ (argv[1], "FE_INVALID"))   exception = FE_INVALID;   else
      if (STREQ (argv[1], "FE_DIVBYZERO")) exception = FE_DIVBYZERO; else
      if (STREQ (argv[1], "FE_OVERFLOW"))  exception = FE_OVERFLOW;  else
      if (STREQ (argv[1], "FE_UNDERFLOW")) exception = FE_UNDERFLOW; else
      if (STREQ (argv[1], "FE_INEXACT"))   exception = FE_INEXACT;   else
        {
          printf ("Invalid argument: %s\n", argv[1]);
          exit (1);
        }

      /* Clear FE_XX exceptions from past operations.  */
      feclearexcept (exception);

      /* An FE_XX exception shall trigger a SIGFPE signal, which by default
         terminates the program.  */
      if (feenableexcept (exception) == -1)
        {
          fputs ("Skipping test: trapping floating-point exceptions are not supported on this machine.\n", stderr);
          return 77;
        }

      feraiseexcept (exception);
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
