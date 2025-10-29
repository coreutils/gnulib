/* Functions for tracking which floating-point exceptions have occurred.
   Copyright (C) 1997-2025 Free Software Foundation, Inc.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of the
   License, or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Based on glibc/sysdeps/<cpu>/{fclrexcpt.c,fraiseexcpt.c,ftestexcept.c}
   together with glibc/sysdeps/<cpu>/{fpu_control.h,fenv_private.h,fenv_libc.h}.  */

#include <config.h>

/* Specification.  */
#include <fenv.h>

#include "fenv-private.h"

_GL_UNUSED static void
generic_feraiseexcept (int exceptions)
{
  /* First: invalid exception.  */
  if (exceptions & FE_INVALID)
    {
      double volatile a;
      _GL_UNUSED double volatile b;
      a = 0; b = a / a;
    }
  /* Next: division by zero.  */
  if (exceptions & FE_DIVBYZERO)
    {
      double volatile a, b;
      _GL_UNUSED double volatile c;
      a = 1; b = 0; c = a / b;
    }
  /* Next: overflow.  */
  if (exceptions & FE_OVERFLOW)
    {
      double volatile a;
      _GL_UNUSED double volatile b;
      a = 1e200; b = a * a;
    }
  /* Next: underflow.  */
  if (exceptions & FE_UNDERFLOW)
    {
      double volatile a;
      _GL_UNUSED double volatile b;
      a = 1e-200; b = a * a;
    }
  /* Last: inexact.  */
  if (exceptions & FE_INEXACT)
    {
      double volatile a, b;
      _GL_UNUSED double volatile c;
      a = 1; b = 3; c = a / b;
    }
}

#if defined __GNUC__ || defined __clang__ || defined _MSC_VER

# if (defined __x86_64__ || defined _M_X64) || (defined __i386 || defined _M_IX86)

int
feraiseexcept (int exceptions)
{
#  if defined _MSC_VER

  /* Setting the exception flags only in the SSE unit (i.e. in the mxcsr
     register) would not cause the hardware to trap on the exception.  */
  generic_feraiseexcept (exceptions);

#  else

  exceptions &= FE_ALL_EXCEPT;

  if ((exceptions & ~(FE_INVALID | FE_DIVBYZERO)) == 0)
    {
      /* Like generic_feraiseexcept (exceptions).  */
      /* This code is probably faster than the general code below.  */
      /* First: invalid exception.  */
      if (exceptions & FE_INVALID)
        {
          double volatile a;
          _GL_UNUSED double volatile b;
          a = 0; b = a / a;
        }
      /* Next: division by zero.  */
      if (exceptions & FE_DIVBYZERO)
        {
          double volatile a, b;
          _GL_UNUSED double volatile c;
          a = 1; b = 0; c = a / b;
        }
    }
  else
    {
      /* The general case.  */

      /* Set the bits in the 387 unit.  */
      x86_387_fenv_t env;
      __asm__ __volatile__ ("fnstenv %0" : "=m" (*&env));
      /* Note: fnstenv masks all floating-point exceptions until the fldenv
         below.  */
      env.__status_word |= exceptions;
      __asm__ __volatile__ ("fldenv %0" : : "m" (*&env));
      /* A trap (if enabled) is triggered only at the next floating-point
         instruction.  Force it to occur here.  */
      __asm__ __volatile__ ("fwait");
    }

#  endif
  return 0;
}

# elif defined __aarch64__ /* arm64 */

int
feraiseexcept (int exceptions)
{
#  if 0
  /* This would just set the flag bits and make fetestexcept() work as expected.
     But it would not cause the hardware to trap on the exception.  */
  exceptions &= FE_ALL_EXCEPT;

  unsigned long fpsr, orig_fpsr;
  _FPU_GETFPSR (orig_fpsr);
  fpsr = orig_fpsr | exceptions;
  if (fpsr != orig_fpsr)
    _FPU_SETFPSR (fpsr);
#  else
  /* This is how glibc does it.
     The drawback is that when FE_OVERFLOW is raised, FE_INEXACT is raised
     with it.  */
  generic_feraiseexcept (exceptions);
#  endif
  return 0;
}

# elif defined __arm__

int
feraiseexcept (int exceptions)
{
#  ifdef __SOFTFP__
  exceptions &= FE_ALL_EXCEPT;

  if (exceptions != 0)
    return -1;
#  else
  /* Raise exceptions represented by EXCEPTIONS.  But we must raise only
     one signal at a time.  It is important that if the overflow/underflow
     exception and the inexact exception are given at the same time,
     the overflow/underflow exception follows the inexact exception.  After
     each exception we read from the fpscr, to force the exception to be
     raised immediately.  */
  /* XXX Probably this should do actual floating-point operations, like in
     generic_feraiseexcept, not just setting flag bits in the fpscr.  */
  unsigned int fpscr, orig_fpscr;
  /* First: invalid exception.  */
  if (exceptions & FE_INVALID)
    {
      _FPU_GETCW (orig_fpscr);
      fpscr = orig_fpscr | FE_INVALID;
      if (fpscr != orig_fpscr)
        {
          _FPU_SETCW (fpscr);
          _FPU_GETCW (fpscr);
        }
    }
  /* Next: division by zero.  */
  if (exceptions & FE_DIVBYZERO)
    {
      _FPU_GETCW (orig_fpscr);
      fpscr = orig_fpscr | FE_DIVBYZERO;
      if (fpscr != orig_fpscr)
        {
          _FPU_SETCW (fpscr);
          _FPU_GETCW (fpscr);
        }
    }
  /* Next: overflow.  */
  if (exceptions & FE_OVERFLOW)
    {
      _FPU_GETCW (orig_fpscr);
      fpscr = orig_fpscr | FE_OVERFLOW;
      if (fpscr != orig_fpscr)
        {
          _FPU_SETCW (fpscr);
          _FPU_GETCW (fpscr);
        }
    }
  /* Next: underflow.  */
  if (exceptions & FE_UNDERFLOW)
    {
      _FPU_GETCW (orig_fpscr);
      fpscr = orig_fpscr | FE_UNDERFLOW;
      if (fpscr != orig_fpscr)
        {
          _FPU_SETCW (fpscr);
          _FPU_GETCW (fpscr);
        }
    }
  /* Last: inexact.  */
  if (exceptions & FE_INEXACT)
    {
      _FPU_GETCW (orig_fpscr);
      fpscr = orig_fpscr | FE_INEXACT;
      if (fpscr != orig_fpscr)
        {
          _FPU_SETCW (fpscr);
          _FPU_GETCW (fpscr);
        }
    }
#  endif
  return 0;
}

# elif defined __alpha

/* Prefer the Linux system call when available.
   See glibc/sysdeps/unix/sysv/linux/alpha/fraiseexcpt.S  */
#  if !defined __linux__
int
feraiseexcept (int exceptions)
{
  /* This implementation cannot raise FE_INEXACT.  */
  generic_feraiseexcept (exceptions);
  return 0;
}
#  endif

# elif defined __hppa

int
feraiseexcept (int exceptions)
{
  generic_feraiseexcept (exceptions);
  return 0;
}

# elif defined __ia64__

int
feraiseexcept (int exceptions)
{
  /* Raise exceptions represented by EXCEPTIONS.  But we must raise only
     one signal at a time.  It is important that if the overflow/underflow
     exception and the inexact exception are given at the same time,
     the overflow/underflow exception precedes the inexact exception.  */
  generic_feraiseexcept (exceptions);
  return 0;
}

# elif defined __m68k__

int
feraiseexcept (int exceptions)
{
  generic_feraiseexcept (exceptions);
  return 0;
}

# elif defined __mips__

int
feraiseexcept (int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;

  /* Set also the cause bits.  The setting of the cause bits is what actually
     causes the hardware to trap on the exception, if the corresponding enable
     bit is set as well.  */
  unsigned int fcsr, orig_fcsr;
  _FPU_GETCW (orig_fcsr);
  fcsr = orig_fcsr | ((exceptions << 10) | exceptions);
  if (fcsr != orig_fcsr)
    _FPU_SETCW (fcsr);

  return 0;
}

# elif defined __loongarch__

int
feraiseexcept (int exceptions)
{
#  if 0
  /* This would just set the flag bits and make fetestexcept() work as expected.
     But it would not cause the hardware to trap on the exception.  */
  exceptions &= FE_ALL_EXCEPT;

  /* Set also the cause bits.  The setting of the cause bits is what actually
     causes the hardware to trap on the exception, if the corresponding enable
     bit is set as well.  */
  unsigned int fcsr, orig_fcsr;
  _FPU_GETCW (orig_fcsr);
  fcsr = orig_fcsr | ((exceptions << 8) | exceptions);
  if (fcsr != orig_fcsr)
    _FPU_SETCW (fcsr);
#  else
  /* This is how glibc does it.
     The drawback is that when FE_OVERFLOW is raised, FE_INEXACT is raised
     with it.  */
  generic_feraiseexcept (exceptions);
#  endif
  return 0;
}

# elif defined _ARCH_PPC

int
feraiseexcept (int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;

  union { unsigned long long u; double f; } memenv, orig_memenv;
  _FPU_GETCW_AS_DOUBLE (memenv.f);
  orig_memenv = memenv;

  /* Instead of setting FE_INVALID (= bit 29), we need to set one of the
     individual bits: bit 10 or, if that does not work, bit 24.  */
  memenv.u |= (exceptions & FE_INVALID
               ? (exceptions & ~FE_INVALID) | (1U << 10)
               : exceptions);

  if (!(memenv.u == orig_memenv.u))
    {
      _FPU_SETCW_AS_DOUBLE (memenv.f);
      if (exceptions & FE_INVALID)
        {
          /* Did it work?  */
          _FPU_GETCW_AS_DOUBLE (memenv.f);
          if ((memenv.u & FE_INVALID) == 0)
            {
              memenv.u |= (1U << 24);
              _FPU_SETCW_AS_DOUBLE (memenv.f);
            }
        }
    }

  return 0;
}

# elif defined __riscv

int
feraiseexcept (int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;
  __asm__ __volatile__ ("csrs fflags, %0" : : "r" (exceptions));
  return 0;
}

# elif defined __s390__ || defined __s390x__

int
feraiseexcept (int exceptions)
{
  generic_feraiseexcept (exceptions);
  return 0;
}

# elif defined __sh__

int
feraiseexcept (int exceptions)
{
#  if 0
  /* This would just set the flag bits and make fetestexcept() work as expected.
     But it would not cause the hardware to trap on the exception.  */
  exceptions &= FE_ALL_EXCEPT;

  unsigned int fpscr, orig_fpscr;
  _FPU_GETCW (orig_fpscr);
  fpscr = orig_fpscr | exceptions;
  if (fpscr != orig_fpscr)
    _FPU_SETCW (fpscr);
#  else
  /* This is how glibc does it.
     The drawback is that when FE_OVERFLOW is raised, FE_INEXACT is raised
     with it.  */
  generic_feraiseexcept (exceptions);
#  endif
  return 0;
}

# elif defined __sparc

int
feraiseexcept (int exceptions)
{
#  if 0
  /* This would just set the flag bits and make fetestexcept() work as expected.
     But it would not cause the hardware to trap on the exception.  */
  exceptions &= FE_ALL_EXCEPT;

  unsigned long fsr, orig_fsr;
  _FPU_GETCW (orig_fsr);
#   if FE_INEXACT == 32 /* glibc compatible FE_* values */
  fsr = orig_fsr | exceptions;
#   else /* Solaris compatible FE_* values */
  fsr = orig_fsr | (exceptions << 5);
#   endif
  if (fsr != orig_fsr)
    _FPU_SETCW (fsr);
#  else
  /* This is how glibc does it.
     The drawback is that when FE_OVERFLOW is raised, FE_INEXACT is raised
     with it.  */
  generic_feraiseexcept (exceptions);
#  endif
  return 0;
}

# else

#  if defined __GNUC__ || defined __clang__
#   warning "Unknown CPU / architecture. Please report your platform and compiler to <bug-gnulib@gnu.org>."
#  endif
#  define NEED_FALLBACK 1

# endif

#else

/* The compiler does not support __asm__ statements or equivalent
   intrinsics.  */

# define NEED_FALLBACK 1

#endif

#if NEED_FALLBACK

/* A fallback that should work everywhere.  */

int
feraiseexcept (int exceptions)
{
  generic_feraiseexcept (exceptions);
  return 0;
}

#endif
