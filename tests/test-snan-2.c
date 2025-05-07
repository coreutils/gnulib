/* Tests of signalling not-a-number.
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
#include "snan.h"

#include <stdio.h>

#include "fpe-trapping.h"

#if HAVE_FPE_TRAPPING

# include <fenv.h>

# include "macros.h"

float volatile resultf;
double volatile resultd;
long double volatile resultl;

int
main (int argc, char *argv[])
{
  /* Fetch the NaN values before we start watching out for FE_INVALID
     exceptions, because the division 0.0 / 0.0 itself also raises an
     FE_INVALID exception.
     The use of 'volatile' prevents the compiler from doing constant-folding
     optimizations on these values.  An alternative, for GCC only, would be
     the command-line option '-fsignaling-nans'.  */
  _GL_UNUSED float volatile nanf = SNaNf ();
  _GL_UNUSED double volatile nand = SNaNd ();
  _GL_UNUSED long double volatile nanl = SNaNl ();

  /* Check that the values are really signalling.  */

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
        /* This test does not work on 32-bit x86 processors, as well as
           on x86_64 processors with CC="gcc -mfpmath=387", because loading
           SNaNf() into a 387 FPU register already converted it to a quiet NaN.
           See <https://lists.gnu.org/archive/html/bug-gnulib/2023-10/msg00060.html>
           for details.  */
        #if !((defined __i386 || defined _M_IX86) \
              || ((defined __x86_64__ || defined _M_X64) && __FLT_EVAL_METHOD__ == 2))
        /* This test does not work on AIX 7.1 with the xlc compiler, even with
           the compiler options -qfloat=fenv -qfloat=nans -qfloat=spnans.  */
        #if !(defined _AIX && defined __xlC__)
        resultf = nanf + 42.0f;
        #else
        fputs ("Skipping test: known failure on this platform with this compiler\n", stderr);
        return 77;
        #endif
        #else
        fputs ("Skipping test: known failure on this platform\n", stderr);
        return 77;
        #endif
        break;

      case 'd':
        /* This test does not work on 32-bit x86 processors, as well as
           on x86_64 processors with CC="gcc -mfpmath=387", because loading
           SNaNd() into a 387 FPU register already converted it to a quiet NaN.
           See <https://lists.gnu.org/archive/html/bug-gnulib/2023-10/msg00060.html>
           for details.  */
        #if !((defined __i386 || defined _M_IX86) \
              || ((defined __x86_64__ || defined _M_X64) && __FLT_EVAL_METHOD__ == 2))
        resultd = nand + 42.0;
        #else
        fputs ("Skipping test: known failure on this platform\n", stderr);
        return 77;
        #endif
        break;

      case 'l':
        /* This test does not work on Linux/alpha with glibc 2.7.  But it
           works with glibc 2.36.  Cause unknown.
           This test does not work on Linux/loongarch64 with glibc 2.37.
           Cause unknown.
           This test does not work on eglibc 2.13/mips64
           (bug in libc function __addtf3).
           This test does not work on FreeBSD/arm64 and OpenBSD/mips64
           (bug in libc function __addtf3).
           This test does not work on FreeBSD/sparc64 and NetBSD/sparc64
           (bug in libc function _Qp_add).
           This test does not work on Cygwin 2.9.0/i386. Cause unknown.
           This test does not work on MSVC/i386, because of the general IA-32
           problem (see above) and 'long double' == 'double'.  */
        #if !((__GLIBC__ == 2 && __GLIBC_MINOR__ < 36 && defined __alpha) \
              || (__GLIBC__ >= 2 && defined __loongarch__) \
              || (((__GLIBC__ == 2 && __GLIBC_MINOR__ < 19 && defined __mips64) \
                   || ((defined __FreeBSD__ || defined __NetBSD__ || defined __OpenBSD__) && (defined __aarch64__ || defined __mips64__ || defined __sparc))) \
                  && !HAVE_SAME_LONG_DOUBLE_AS_DOUBLE) \
              || (defined __CYGWIN__ && defined __i386) \
              || (((defined __i386 || defined _M_IX86) \
                   || ((defined __x86_64__ || defined _M_X64) && __FLT_EVAL_METHOD__ == 2)) \
                  && HAVE_SAME_LONG_DOUBLE_AS_DOUBLE))
        resultl = nanl + 42.0L;
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

/* No HAVE_FPE_TRAPPING available.
   We could use the various alternative approaches from
   libgfortran/config/fpu-*.h, but that's not worth it.  */

int
main ()
{
  fputs ("Skipping test: feenableexcept not available\n", stderr);
  return 77;
}

#endif
