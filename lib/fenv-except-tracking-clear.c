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

#if defined __GNUC__ || defined __clang__ || defined _MSC_VER

# if (defined __x86_64__ || defined _M_X64) || (defined __i386 || defined _M_IX86)

int
feclearexcept (int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;

  exceptions = exceptions_to_x86hardware (exceptions);

#  if defined _MSC_VER

  /* Clear the bits only in the SSE unit.  */
  unsigned int mxcsr, orig_mxcsr;
  _FPU_GETSSECW (orig_mxcsr);
  mxcsr = orig_mxcsr & ~exceptions;
  if (mxcsr != orig_mxcsr)
    _FPU_SETSSECW (mxcsr);

#  else

  /* Clear the bits in the 387 unit.  */
  x86_387_fenv_t env;
  __asm__ __volatile__ ("fnstenv %0" : "=m" (*&env));
  /* Note: fnstenv masks all floating-point exceptions until the fldenv
     below.  */
  env.__status_word &= ~exceptions;
  __asm__ __volatile__ ("fldenv %0" : : "m" (*&env));

  if (CPU_HAS_SSE ())
    {
      /* Clear the bits in the SSE unit as well.  */
      unsigned int mxcsr, orig_mxcsr;
      _FPU_GETSSECW (orig_mxcsr);
      mxcsr = orig_mxcsr & ~exceptions;
      if (mxcsr != orig_mxcsr)
        _FPU_SETSSECW (mxcsr);
    }

#  endif

  return 0;
}

# elif defined __aarch64__ /* arm64 */

int
feclearexcept (int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;

  unsigned long fpsr, orig_fpsr;
  _FPU_GETFPSR (orig_fpsr);
  fpsr = orig_fpsr & ~exceptions;
  if (fpsr != orig_fpsr)
    _FPU_SETFPSR (fpsr);

  return 0;
}

# elif defined __arm__

int
feclearexcept (int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;

#  ifdef __SOFTFP__
  if (exceptions != 0)
    return -1;
#  else
  unsigned int fpscr, orig_fpscr;
  _FPU_GETCW (orig_fpscr);
  fpscr = orig_fpscr & ~exceptions;
  if (fpscr != orig_fpscr)
    _FPU_SETCW (fpscr);
#  endif
  return 0;
}

# elif defined __alpha

int
feclearexcept (int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;

  unsigned long swcr, orig_swcr;
  orig_swcr = __ieee_get_fp_control ();
  swcr = orig_swcr & ~exceptions;
  if (swcr != orig_swcr)
    __ieee_set_fp_control (swcr);

  return 0;
}

# elif defined __hppa

int
feclearexcept (int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;

  union { unsigned long long fpreg; unsigned int halfreg[2]; } s;
  /* Get the current status word. */
  __asm__ __volatile__ ("fstd %%fr0,0(%1)" : "=m" (s.fpreg) : "r" (&s.fpreg) : "%r0");
  unsigned int old_halfreg0 = s.halfreg[0];
  /* Clear all the relevant bits. */
  s.halfreg[0] &= ~ ((unsigned int) exceptions << 27);
  if (s.halfreg[0] != old_halfreg0)
    {
      /* Store the new status word.  */
      __asm__ __volatile__ ("fldd 0(%0),%%fr0" : : "r" (&s.fpreg), "m" (s.fpreg) : "%r0");
    }

  return 0;
}

# elif defined __ia64__

int
feclearexcept (int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;

  unsigned long fpsr, orig_fpsr;
  _FPU_GETCW (orig_fpsr);
  fpsr = orig_fpsr & ~ (unsigned long) (exceptions << 13);
  if (fpsr != orig_fpsr)
    _FPU_SETCW (fpsr);

  return 0;
}

# elif defined __m68k__

int
feclearexcept (int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;

  unsigned int fpsr, orig_fpsr;
  _FPU_GETFPSR (orig_fpsr);
  fpsr = orig_fpsr & ~ exceptions;
  if (fpsr != orig_fpsr)
    _FPU_SETFPSR (fpsr);

  return 0;
}

# elif defined __mips__

int
feclearexcept (int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;

  /* Clear also the cause bits.  If the cause bit is not cleared, the next
     CTC instruction (just below) will re-generate the exception.  */
  unsigned int fcsr, orig_fcsr;
  _FPU_GETCW (orig_fcsr);
  fcsr = orig_fcsr & ~ ((exceptions << 10) | exceptions);
  if (fcsr != orig_fcsr)
    _FPU_SETCW (fcsr);

  return 0;
}

# elif defined __loongarch__

int
feclearexcept (int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;

  /* Clear also the cause bits.  If the cause bit is not cleared, the next
     CTC instruction (just below) will re-generate the exception.  */
  unsigned int fcsr, orig_fcsr;
  _FPU_GETCW (orig_fcsr);
  fcsr = orig_fcsr & ~ ((exceptions << 8) | exceptions);
  if (fcsr != orig_fcsr)
    _FPU_SETCW (fcsr);

  return 0;
}

# elif defined _ARCH_PPC

int
feclearexcept (int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;

  union { unsigned long long u; double f; } memenv, orig_memenv;
  _FPU_GETCW_AS_DOUBLE (memenv.f);
  orig_memenv = memenv;

  /* Instead of clearing FE_INVALID (= bit 29), we need to clear the
     individual bits.  */
  memenv.u &= ~ (exceptions & FE_INVALID
                 ? (exceptions & ~FE_INVALID) | 0x01F80700U
                 : exceptions);

  if (!(memenv.u == orig_memenv.u))
    _FPU_SETCW_AS_DOUBLE (memenv.f);

  return 0;
}

# elif defined __riscv

int
feclearexcept (int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;
  __asm__ __volatile__ ("csrc fflags, %0" : : "r" (exceptions));
  return 0;
}

# elif defined __s390__ || defined __s390x__

int
feclearexcept (int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;

  unsigned int fpc, orig_fpc;
  _FPU_GETCW (orig_fpc);
#  if FE_INEXACT == 8 /* glibc compatible FE_* values */
  fpc = orig_fpc & ~(exceptions << 16);
  if ((fpc & 0x00000300) == 0)
    fpc &= ~(exceptions << 8);
#  else /* musl libc compatible FE_* values */
  fpc = orig_fpc & ~exceptions;
  if ((fpc & 0x00000300) == 0)
    fpc &= ~(exceptions >> 8);
#  endif
  if (fpc != orig_fpc)
    _FPU_SETCW (fpc);

  return 0;
}

# elif defined __sh__

int
feclearexcept (int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;

  unsigned int fpscr, orig_fpscr;
  _FPU_GETCW (orig_fpscr);
  fpscr = orig_fpscr & ~exceptions;
  if (fpscr != orig_fpscr)
    _FPU_SETCW (fpscr);

  return 0;
}

# elif defined __sparc

int
feclearexcept (int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;

  unsigned long fsr, orig_fsr;
  _FPU_GETCW (orig_fsr);
#  if FE_INEXACT == 32 /* glibc compatible FE_* values */
  fsr = orig_fsr & ~exceptions;
#  else /* Solaris compatible FE_* values */
  fsr = orig_fsr & ~(exceptions << 5);
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

# if HAVE_FPSETSTICKY
/* FreeBSD ≥ 3.1, NetBSD ≥ 1.1, OpenBSD, Solaris, Minix ≥ 3.2.  */

/* Get fpgetsticky, fpsetsticky.  */
#  include <ieeefp.h>
/* The type is called 'fp_except_t' on FreeBSD, but 'fp_except' on
   all other systems.  */
#  if !defined __FreeBSD__
#   define fp_except_t fp_except
#  endif

int
feclearexcept (int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;

  fp_except_t flags, orig_flags;
  orig_flags = fpgetsticky ();
  flags = orig_flags & ~exceptions;
  if (flags != orig_flags)
    fpsetsticky (flags);

  return 0;
}

# elif defined _AIX && defined _ARCH_PPC /* AIX */

#  include <float.h>
#  include <fpxcp.h>

/* Documentation:
   <https://www.ibm.com/docs/en/aix/7.3?topic=f-fp-clr-flag-fp-set-flag-fp-read-flag-fp-swap-flag-subroutine>  */

int
feclearexcept (int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;

  /* In addition to clearing FE_INVALID (= bit 29), we also need to clear the
     individual bits.  */
  fpflag_t f_to_clear =
    exceptions_to_fpflag (exceptions)
    | (exceptions & FE_INVALID ? 0x01F80700U : 0);
  if (f_to_clear != 0)
    fp_clr_flag (f_to_clear);

  return 0;
}

# else

#  define NEED_FALLBACK 1

# endif

#endif

#if NEED_FALLBACK

/* A dummy fallback.  */

int
feclearexcept (int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;
  if (exceptions != 0)
    return -1;
  return 0;
}

#endif
