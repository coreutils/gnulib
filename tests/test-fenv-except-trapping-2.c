/* Test of turning floating-point exceptions into traps (signals).
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

#include "infinity.h"
#include "snan.h"
#include "macros.h"

/* Operations that should raise various floating-point exceptions.  */

/* For a list, see the glibc documentation
   <https://www.gnu.org/software/libc/manual/html_node/FP-Exceptions.html>  */

#define infinityf my_infinityf /* Avoid collision with Cygwin's <math.h> */
static float volatile infinityf, qnanf;
static double volatile infinityd, qnand;
static long double volatile infinityl, qnanl;

static void
raise_invalid_addition (char type)
{
  switch (type)
    {
    case 'f':
      {
        float volatile a, b;
        _GL_UNUSED float volatile c;
        a = infinityf; b = - infinityf;
        c = a + b;
      }
      break;
    case 'd':
      {
        double volatile a, b;
        _GL_UNUSED double volatile c;
        a = infinityd; b = - infinityd;
        c = a + b;
      }
      break;
    case 'l':
      {
        long double volatile a, b;
        _GL_UNUSED long double volatile c;
        a = infinityl; b = - infinityl;
        c = a + b;
      }
      break;
    }
}

static void
raise_invalid_multiplication (char type)
{
  switch (type)
    {
    case 'f':
      {
        float volatile a, b;
        _GL_UNUSED float volatile c;
        a = infinityf; b = 0.0f;
        c = a * b;
      }
      break;
    case 'd':
      {
        double volatile a, b;
        _GL_UNUSED double volatile c;
        a = infinityd; b = 0.0;
        c = a * b;
      }
      break;
    case 'l':
      {
        long double volatile a, b;
        _GL_UNUSED long double volatile c;
        a = infinityl; b = 0.0L;
        c = a * b;
      }
      break;
    }
}

static void
raise_invalid_division (char type)
{
  switch (type)
    {
    case 'f':
      {
        float volatile a, b;
        _GL_UNUSED float volatile c;
        a = infinityf; b = - infinityf;
        c = a / b;
      }
      break;
    case 'd':
      {
        double volatile a, b;
        _GL_UNUSED double volatile c;
        a = infinityd; b = - infinityd;
        c = a / b;
      }
      break;
    case 'l':
      {
        long double volatile a, b;
        _GL_UNUSED long double volatile c;
        a = infinityl; b = - infinityl;
        c = a / b;
      }
      break;
    }
}

static void
raise_invalid_comparison (char type)
{
  switch (type)
    {
    case 'f':
      {
        float volatile a, b;
        _GL_UNUSED int volatile c;
        a = qnanf; b = qnanf;
        c = a > b;
      }
      break;
    case 'd':
      {
        double volatile a, b;
        _GL_UNUSED int volatile c;
        a = qnand; b = qnand;
        c = a > b;
      }
      break;
    case 'l':
      {
        long double volatile a, b;
        _GL_UNUSED int volatile c;
        a = qnanl; b = qnanl;
        c = a > b;
      }
      break;
    }
}

static void
raise_invalid_snan (char type)
{
  switch (type)
    {
    case 'f':
      {
        float volatile a, b;
        _GL_UNUSED float volatile c;
        a = SNaNf (); b = 1.0f;
        c = a + b;
      }
      break;
    case 'd':
      {
        double volatile a, b;
        _GL_UNUSED double volatile c;
        a = SNaNd (); b = 1.0;
        c = a + b;
      }
      break;
    case 'l':
      {
        long double volatile a, b;
        _GL_UNUSED long double volatile c;
        a = SNaNl (); b = 1.0L;
        c = a + b;
      }
      break;
    }
}

static void
raise_divbyzero (char type)
{
  switch (type)
    {
    case 'f':
      {
        float volatile a, b;
        _GL_UNUSED float volatile c;
        a = 2.5f; b = - 0.0f;
        c = a / b;
      }
      break;
    case 'd':
      {
        double volatile a, b;
        _GL_UNUSED double volatile c;
        a = 2.5; b = - 0.0;
        c = a / b;
      }
      break;
    case 'l':
      {
        long double volatile a, b;
        _GL_UNUSED long double volatile c;
        a = 2.5L; b = - 0.0L;
        c = a / b;
      }
      break;
    }
}

static void
raise_overflow (char type)
{
  switch (type)
    {
    case 'f':
      {
        float volatile a, b;
        _GL_UNUSED float volatile c;
        a = 1e20f; b = 1e30f;
        c = a * b;
      }
      break;
    case 'd':
      {
        double volatile a, b;
        _GL_UNUSED double volatile c;
        a = 1e160; b = 1e260;
        c = a * b;
      }
      break;
    case 'l':
      {
        long double volatile a, b;
        _GL_UNUSED long double volatile c;
        a = 1e200L; b = 1e300L;
        c = a * b;
        c = c * c;
        c = c * c;
        c = c * c;
        c = c * c;
      }
      break;
    }
}

static void
raise_underflow (char type)
{
  switch (type)
    {
    case 'f':
      {
        float volatile a, b;
        _GL_UNUSED float volatile c;
        a = 1e-20f; b = 1e-30f;
        c = a * b;
      }
      break;
    case 'd':
      {
        double volatile a, b;
        _GL_UNUSED double volatile c;
        a = 1e-160; b = 1e-260;
        c = a * b;
      }
      break;
    case 'l':
      {
        long double volatile a, b;
        _GL_UNUSED long double volatile c;
        a = 1e-200L; b = 1e-300L;
        c = a * b;
        c = c * c;
        c = c * c;
        c = c * c;
        c = c * c;
      }
      break;
    }
}

static void
raise_inexact (char type)
{
  switch (type)
    {
    case 'f':
      {
        float volatile a, b;
        _GL_UNUSED float volatile c;
        a = 2.5f; b = 3.0f;
        c = a / b;
      }
      break;
    case 'd':
      {
        double volatile a, b;
        _GL_UNUSED double volatile c;
        a = 2.5; b = 3.0;
        c = a / b;
      }
      break;
    case 'l':
      {
        long double volatile a, b;
        _GL_UNUSED long double volatile c;
        a = 2.5L; b = 3.0L;
        c = a / b;
      }
      break;
    }
}

int
main (int argc, char *argv[])
{
  if (argc > 3)
    {
      const char *operation_arg = argv[1];
      const char *procedure_arg = argv[2];
      const char *type_arg = argv[3];

      void (*operation) (char) = NULL;
      int expected_exceptions = 0;
      int possible_exceptions = 0;

      /* Preparations (to be executed before we call feenableexcept).  */
      infinityf = Infinityf ();
      qnanf = NaNf ();
      infinityd = Infinityd ();
      qnand = NaNd ();
      infinityl = Infinityl ();
      qnanl = NaNl ();
      feclearexcept (FE_ALL_EXCEPT);

      /* Analyze the operation argument.  */
      switch (operation_arg[0])
        {
        case '1':
          operation = raise_invalid_addition;
          expected_exceptions = FE_INVALID;
          break;
        case '2':
          operation = raise_invalid_multiplication;
          expected_exceptions = FE_INVALID;
          break;
        case '3':
          operation = raise_invalid_division;
          expected_exceptions = FE_INVALID;
          break;
        case '4':
          operation = raise_invalid_comparison;
          expected_exceptions = FE_INVALID;
          break;
        case '5':
          operation = raise_invalid_snan;
          expected_exceptions = FE_INVALID;
          break;
        case '6':
          operation = raise_divbyzero;
          expected_exceptions = FE_DIVBYZERO;
          break;
        case '7':
          operation = raise_overflow;
          expected_exceptions = FE_OVERFLOW;
          possible_exceptions = FE_INEXACT;
          break;
        case '8':
          operation = raise_underflow;
          expected_exceptions = FE_UNDERFLOW;
          possible_exceptions = FE_INEXACT;
          break;
        case '9':
          operation = raise_inexact;
          expected_exceptions = FE_INEXACT;
          break;
        }

      /* Analyze the procedure argument.  */
      switch (procedure_arg[0])
        {
        /* These three procedures should lead to a trap.  */
        case 'p':
          if (feenableexcept (expected_exceptions) == -1)
            goto skip;
          break;
        case 'q':
          if (feenableexcept (FE_ALL_EXCEPT) == -1)
            goto skip;
          break;
        case 'r':
          if (feenableexcept (FE_ALL_EXCEPT) == -1)
            goto skip;
          ASSERT (fedisableexcept (FE_ALL_EXCEPT & ~expected_exceptions)
                  == FE_ALL_EXCEPT);
          break;
        /* This procedure should *not* lead to a trap.  */
        case 's':
          if (feenableexcept (FE_ALL_EXCEPT & ~(expected_exceptions | possible_exceptions)) == -1)
            goto skip;
          break;
        }

      /* Avoid known test failures.  */
      int known_failure = 0;
      /* The '4' tests do not work
         - on glibc/{i386,x86_64}, with gcc < 8 (except when option -mno-ieee-fp
           is used) or with clang,
         - on glibc/powerpc* and glibc/s390*,
         - as well as on
           GNU/kFreeBSD/i386, GNU/kFreeBSD/x86_64,
           musl libc/i386, musl libc/powerpc64le,
           macOS/i386, macOS/x86_64, macOS/arm64,
           FreeBSD/i386, FreeBSD/x86_64, FreeBSD/powerpc64,
           NetBSD/i386, NetBSD/x86_64, NetBSD/powerpc,
           OpenBSD/i386, OpenBSD/x86_64, OpenBSD/mips64,
           Minix/i386,
           AIX/powerpc,
           Solaris/i386, Solaris/x86_64,
           Cygwin/x86_64,
           native Windows/i386, native Windows/x86_64,
           Haiku/i386.
         Explanation of some of the {i386,x86_64} cases:
         - Quoting the Intel 64 and IA-32 Architectures Software Developer's
           Manual:
           "The UCOMISD instruction differs from the COMISD instruction in that
            it signals a SIMD floating-point invalid operation exception (#I)
            only when a source operand is an SNaN. The COMISD instruction
            signals an invalid numeric exception only if a source operand is
            either an SNaN or a QNaN."
         - gcc < 8 (except when option -mno-ieee-fp is used) and clang generate
           'ucom*' or 'fucom*' instructions and thus fail the test.
         - gcc >= 8 generates 'com*' or 'fcom*' instructions and thus passes
           the test.  */
      #if (__GLIBC__ >= 2 && ((defined __x86_64__ || defined _M_X64) || (defined __i386 || defined _M_IX86)) && (__GNUC__ < 8 || defined __clang__)) \
          || (__GLIBC__ >= 2 && (defined _ARCH_PPC || (defined __s390__ || defined __s390x__))) \
          || (__GLIBC__ >= 2 && __FreeBSD_kernel__ && ((defined __x86_64__ || defined _M_X64) || (defined __i386 || defined _M_IX86))) \
          || (defined MUSL_LIBC && ((defined __i386 || defined _M_IX86) || defined _ARCH_PPC)) \
          || ((defined __APPLE__ && defined __MACH__) && ((defined __x86_64__ || defined _M_X64) || (defined __i386 || defined _M_IX86) || defined __aarch64__)) \
          || ((defined __FreeBSD__ || defined __NetBSD__ || defined __OpenBSD__) && ((defined __x86_64__ || defined _M_X64) || (defined __i386 || defined _M_IX86))) \
          || ((defined __FreeBSD__ || defined __NetBSD__) && defined _ARCH_PPC) \
          || (defined __OpenBSD__ && defined __mips64) \
          || (defined __minix && (defined __i386 || defined _M_IX86)) \
          || (defined _AIX && defined _ARCH_PPC) \
          || (defined __sun && ((defined __x86_64__ || defined _M_X64) || (defined __i386 || defined _M_IX86))) \
          || (defined __CYGWIN__ && (defined __x86_64__ || defined _M_X64)) \
          || (defined _WIN32 && ((defined __x86_64__ || defined _M_X64) || (defined __i386 || defined _M_IX86))) \
          || (defined __HAIKU__ && (defined __i386 || defined _M_IX86))
      known_failure |= (operation_arg[0] == '4');
      #endif
      /* The '7' and '8' tests, with types 'f' and 'd', do not work reliably
         on Linux/i386.  */
      #if defined __i386 || defined _M_IX86
      known_failure |= (operation_arg[0] == '7' || operation_arg[0] == '8');
      #endif
      /* The '9' tests do not work on Linux/alpha.  */
      #if (__GLIBC__ >= 2 && defined __alpha)
      known_failure |= (operation_arg[0] == '9');
      #endif
      /* The 'l' tests do not work on Linux/loongarch64 with glibc 2.37.
         Likewise on Linux/alpha with glibc 2.7 on Linux 2.6.26.
         Likewise on FreeBSD 12.2/sparc, NetBSD 8.0/sparc, OpenBSD 7.2/sparc64.
         Likewise on OpenBSD 7.4/mips64.
         Cause unknown.  */
      #if (__GLIBC__ >= 2 && defined __loongarch__) \
          || ((__GLIBC__ == 2 && __GLIBC_MINOR__ < 36) && defined __alpha) \
          || ((defined __FreeBSD__ || defined __NetBSD__ || defined __OpenBSD__) && defined __sparc) \
          || (defined __OpenBSD__ && defined __mips64)
      known_failure |= (type_arg[0] == 'l');
      #endif
      if (known_failure)
        {
          if (test_exit_status != EXIT_SUCCESS)
            return test_exit_status;
          fputs ("Skipping test: known failure on this platform\n", stderr);
          return 77;
        }

      /* Analyze the type argument.  */
      switch (type_arg[0])
        {
        case 'f':
        case 'd':
        case 'l':
          operation (type_arg[0]);
          break;
        }
    }

  return test_exit_status;

 skip:
  if (test_exit_status != EXIT_SUCCESS)
    return test_exit_status;
  fputs ("Skipping test: trapping floating-point exceptions are not supported on this machine.\n", stderr);
  return 77;
}
