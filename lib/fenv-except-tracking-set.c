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

/* Based on glibc/sysdeps/<cpu>/fesetexcept.c
   together with glibc/sysdeps/<cpu>/{fpu_control.h,fenv_private.h,fenv_libc.h}.  */

#include <config.h>

/* Specification.  */
#include <fenv.h>

#include "fenv-private.h"

#if defined __GNUC__ || defined __clang__ || defined _MSC_VER

# if (defined __x86_64__ || defined _M_X64) || (defined __i386 || defined _M_IX86)

int
fesetexcept (int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;

  exceptions = exceptions_to_x86hardware (exceptions);

#  if defined _MSC_VER

  /* Set the flags in the SSE unit.  */
  unsigned int mxcsr, orig_mxcsr;
  _FPU_GETSSECW (orig_mxcsr);
  mxcsr = orig_mxcsr | exceptions;
  if (mxcsr != orig_mxcsr)
    _FPU_SETSSECW (mxcsr);

#  else

  /* We can set the flags in the 387 unit or in the SSE unit.
     Either works, due to the way fetestexcept() is implemented.
     Choose the simplest approach.  */
#   if defined __x86_64__ || defined _M_X64
  /* Set the flags in the SSE unit.  */
  unsigned int mxcsr, orig_mxcsr;
  _FPU_GETSSECW (orig_mxcsr);
  mxcsr = orig_mxcsr | exceptions;
  if (mxcsr != orig_mxcsr)
    _FPU_SETSSECW (mxcsr);
#   else
  if (CPU_HAS_SSE ())
    {
      /* Set the flags in the SSE unit.  */
      unsigned int mxcsr, orig_mxcsr;
      _FPU_GETSSECW (orig_mxcsr);
      mxcsr = orig_mxcsr | exceptions;
      if (mxcsr != orig_mxcsr)
        _FPU_SETSSECW (mxcsr);
    }
  else
    {
      /* Set the flags in the 387 unit.  */
      x86_387_fenv_t env;
      __asm__ __volatile__ ("fnstenv %0" : "=m" (*&env));
      /* Note: fnstenv masks all floating-point exceptions until the fldenv
         or fldcw below.  */
      env.__status_word |= exceptions;
      if ((~env.__control_word) & exceptions)
        {
          /* Setting the exception flags may trigger a trap (at the next
             floating-point instruction, but that does not matter).
             ISO C 23 § 7.6.4.4 does not allow it.  */
          __asm__ __volatile__ ("fldcw %0" : : "m" (*&env.__control_word));
          return -1;
        }
      __asm__ __volatile__ ("fldenv %0" : : "m" (*&env));
    }
#   endif
#  endif

  return 0;
}

# elif defined __aarch64__ /* arm64 */

int
fesetexcept (int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;

  unsigned long fpsr, orig_fpsr;
  _FPU_GETFPSR (orig_fpsr);
  fpsr = orig_fpsr | exceptions;
  if (fpsr != orig_fpsr)
    _FPU_SETFPSR (fpsr);

  return 0;
}

# elif defined __arm__

int
fesetexcept (int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;

#  ifdef __SOFTFP__
  if (exceptions != 0)
    return -1;
#  else
  unsigned int fpscr, orig_fpscr;
  _FPU_GETCW (orig_fpscr);
  fpscr = orig_fpscr | exceptions;
  if (fpscr != orig_fpscr)
    _FPU_SETCW (fpscr);
#  endif
  return 0;
}

# elif defined __alpha

int
fesetexcept (int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;

  unsigned long swcr, orig_swcr;
  orig_swcr = __ieee_get_fp_control ();
  swcr = orig_swcr | exceptions;
  if (swcr != orig_swcr)
    __ieee_set_fp_control (swcr);

  return 0;
}

# elif defined __hppa

int
fesetexcept (int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;

  union { unsigned long long fpreg; unsigned int halfreg[2]; } s;
  /* Get the current status word. */
  __asm__ __volatile__ ("fstd %%fr0,0(%1)" : "=m" (s.fpreg) : "r" (&s.fpreg) : "%r0");
  unsigned int old_halfreg0 = s.halfreg[0];
  /* Clear all the relevant bits. */
  s.halfreg[0] |= ((unsigned int) exceptions << 27);
  if (s.halfreg[0] != old_halfreg0)
    {
      /* Store the new status word.  */
      __asm__ __volatile__ ("fldd 0(%0),%%fr0" : : "r" (&s.fpreg), "m" (s.fpreg) : "%r0");
    }

  return 0;
}

# elif defined __ia64__

int
fesetexcept (int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;

  unsigned long fpsr, orig_fpsr;
  _FPU_GETCW (orig_fpsr);
  fpsr = orig_fpsr | (unsigned long) (exceptions << 13);
  if (fpsr != orig_fpsr)
    _FPU_SETCW (fpsr);

  return 0;
}

# elif defined __m68k__

int
fesetexcept (int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;

  unsigned int fpsr, orig_fpsr;
  _FPU_GETFPSR (orig_fpsr);
  fpsr = orig_fpsr | exceptions;
  if (fpsr != orig_fpsr)
    _FPU_SETFPSR (fpsr);

  return 0;
}

# elif defined __mips__

int
fesetexcept (int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;

  unsigned int fcsr, orig_fcsr;
  _FPU_GETCW (orig_fcsr);
  fcsr = orig_fcsr | exceptions;
  if (fcsr != orig_fcsr)
    _FPU_SETCW (fcsr);

  return 0;
}

# elif defined __loongarch__

int
fesetexcept (int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;

  unsigned int fcsr, orig_fcsr;
  _FPU_GETCW (orig_fcsr);
  fcsr = orig_fcsr | exceptions;
  if (fcsr != orig_fcsr)
    _FPU_SETCW (fcsr);

  return 0;
}

# elif defined _ARCH_PPC

int
fesetexcept (int exceptions)
{
  /* The hardware does not support setting an exception flag without triggering
     a trap, except through the "Ignore Exceptions Mode", bits FE0 and FE1 of
     the MSR register set to zero, that can be obtained through a system call:
       - On Linux and NetBSD: prctl (PR_SET_FPEXC, PR_FP_EXC_DISABLED);
       - On AIX: fp_trap (FP_TRAP_OFF);
     But that is not what we need here, as it would have a persistent effect on
     the thread.  */
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
      if (memenv.u & (exceptions >> 22))
        {
          /* Setting the exception flags may trigger a trap.
             ISO C 23 § 7.6.4.4 does not allow it.  */
          return -1;
        }
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
fesetexcept (int exceptions)
{
  /* This is identical to feraiseexcept(), because the hardware does not
     support trapping on floating-point exceptions.  */
  exceptions &= FE_ALL_EXCEPT;
  __asm__ __volatile__ ("csrs fflags, %0" : : "r" (exceptions));
  return 0;
}

# elif defined __s390__ || defined __s390x__

int
fesetexcept (int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;

  unsigned int fpc, orig_fpc;
  _FPU_GETCW (orig_fpc);
#  if FE_INEXACT == 8 /* glibc compatible FE_* values */
  fpc = orig_fpc | (exceptions << 16);
#  else /* musl libc compatible FE_* values */
  fpc = orig_fpc | exceptions;
#  endif
  if (fpc != orig_fpc)
    _FPU_SETCW (fpc);

  return 0;
}

# elif defined __sh__

int
fesetexcept (int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;

  unsigned int fpscr, orig_fpscr;
  _FPU_GETCW (orig_fpscr);
  fpscr = orig_fpscr | exceptions;
  if (fpscr != orig_fpscr)
    _FPU_SETCW (fpscr);

  return 0;
}

# elif defined __sparc

int
fesetexcept (int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;

  unsigned long fsr, orig_fsr;
  _FPU_GETCW (orig_fsr);
#  if FE_INEXACT == 32 /* glibc compatible FE_* values */
  fsr = orig_fsr | exceptions;
#  else /* Solaris compatible FE_* values */
  fsr = orig_fsr | (exceptions << 5);
#  endif
  if (fsr != orig_fsr)
    _FPU_SETCW (fsr);

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

# if defined __sun && ((defined __x86_64__ || defined _M_X64) || (defined __i386 || defined _M_IX86)) && defined __SUNPRO_C
/* Solaris/i386, Solaris/x86_64.  */

/* On these platforms, fpsetsticky cannot be used here, because it may generate
   traps (since fpsetsticky calls _putsw, which modifies the control word of the
   387 unit).  Instead, we need to modify only the flags in the SSE unit.  */

/* Accessors for the mxcsr register.  Fortunately, the Solaris cc supports a
   poor form of 'asm'.  */

static void
getssecw (unsigned int *mxcsr_p)
{
#  if defined __x86_64__ || defined _M_X64
  asm ("stmxcsr (%rdi)");
#  else
  /* The compiler generates a stack frame.  Therefore the first argument is in
     8(%ebp), not in 4(%esp).  */
  asm ("movl 8(%ebp),%eax");
  asm ("stmxcsr (%eax)");
#  endif
}

static void
setssecw (unsigned int const *mxcsr_p)
{
#  if defined __x86_64__ || defined _M_X64
  asm ("ldmxcsr (%rdi)");
#  else
  /* The compiler generates a stack frame.  Therefore the first argument is in
     8(%ebp), not in 4(%esp).  */
  asm ("movl 8(%ebp),%eax");
  asm ("ldmxcsr (%eax)");
#  endif
}

int
fesetexcept (int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;

  /* Set the flags in the SSE unit.  */
  unsigned int mxcsr, orig_mxcsr;
  getssecw (&orig_mxcsr);
  mxcsr = orig_mxcsr | exceptions;
  if (mxcsr != orig_mxcsr)
    setssecw (&mxcsr);

  return 0;
}

# elif HAVE_FPSETSTICKY
/* FreeBSD ≥ 3.1, NetBSD ≥ 1.1, OpenBSD, Solaris, Minix ≥ 3.2.  */

/* Get fpgetsticky, fpsetsticky.  */
#  include <ieeefp.h>
/* The type is called 'fp_except_t' on FreeBSD, but 'fp_except' on
   all other systems.  */
#  if !defined __FreeBSD__
#   define fp_except_t fp_except
#  endif

int
fesetexcept (int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;

  fp_except_t flags, orig_flags;
  orig_flags = fpgetsticky ();
  flags = orig_flags | exceptions;
  if (flags != orig_flags)
    fpsetsticky (flags);

  return 0;
}

# elif defined _AIX && defined _ARCH_PPC /* AIX */

#  include <float.h>
#  include <fpxcp.h>

#  include <fptrap.h>

/* Documentation:
   <https://www.ibm.com/docs/en/aix/7.3?topic=f-fp-clr-flag-fp-set-flag-fp-read-flag-fp-swap-flag-subroutine>  */

int
fesetexcept (int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;

  /* Instead of setting FE_INVALID (= bit 29), we need to set one of the
     individual bits: bit 10 or, if that does not work, bit 24.  */
  fpflag_t f_to_set =
    (exceptions & FE_INVALID
     ? exceptions_to_fpflag (exceptions & ~FE_INVALID) | (1U << 10)
     : exceptions_to_fpflag (exceptions));
  if (f_to_set != 0)
    {
      if ((fegetexcept_impl () & exceptions) != 0)
        {
          /* Setting the exception flags may trigger a trap.
             ISO C 23 § 7.6.4.4 does not allow it.  */
          return -1;
        }
      fp_set_flag (f_to_set);
      if (exceptions & FE_INVALID)
        {
          /* Did it work?  */
          if ((fp_read_flag () & FP_INVALID) == 0)
            fp_set_flag (1U << 24);
        }
    }

  return 0;
}

# else

#  define NEED_FALLBACK 1

# endif

#endif

#if NEED_FALLBACK

/* A dummy fallback.  */

int
fesetexcept (int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;
  if (exceptions != 0)
    return -1;
  return 0;
}

#endif
