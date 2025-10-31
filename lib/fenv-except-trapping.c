/* Functions for turning floating-point exceptions into traps (signals).
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

/* Based on glibc/sysdeps/<cpu>/{feenablxcpt.c,fedisblxcpt.c,fegetexcept.c}
   together with glibc/sysdeps/<cpu>/{fpu_control.h,fenv_private.h,fenv_libc.h}.  */

#include <config.h>

/* Specification.  */
#include <fenv.h>

#include "fenv-private.h"

#if defined __GNUC__ || defined __clang__ || defined _MSC_VER

# if (defined __x86_64__ || defined _M_X64) || (defined __i386 || defined _M_IX86)

int
feenableexcept (int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;

  exceptions = exceptions_to_x86hardware (exceptions);

#  if defined _MSC_VER

  /* Enable the traps only in the SSE unit.  */
  unsigned int mxcsr, orig_mxcsr;
  _FPU_GETSSECW (orig_mxcsr);
  mxcsr = orig_mxcsr & ~(exceptions << 7);
  if (mxcsr != orig_mxcsr)
    _FPU_SETSSECW (mxcsr);

  unsigned int trapbits = 0x3f & ~(orig_mxcsr >> 7);

#  else

  /* Enable the traps in the 387 unit.  */
  unsigned short fctrl, orig_fctrl;
  _FPU_GETCW (orig_fctrl);
  fctrl = orig_fctrl & ~exceptions;
  if (fctrl != orig_fctrl)
    _FPU_SETCW (fctrl);

  if (CPU_HAS_SSE ())
    {
      /* Enable the traps in the SSE unit as well.  */
      unsigned int mxcsr, orig_mxcsr;
      _FPU_GETSSECW (orig_mxcsr);
      mxcsr = orig_mxcsr & ~(exceptions << 7);
      if (mxcsr != orig_mxcsr)
        _FPU_SETSSECW (mxcsr);
    }

  unsigned int trapbits = 0x3f & ~orig_fctrl;

#  endif

  return x86hardware_to_exceptions (trapbits);
}

int
fedisableexcept (int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;

  exceptions = exceptions_to_x86hardware (exceptions);

#  if defined _MSC_VER

  /* Disable the traps only in the SSE unit.  */
  unsigned int mxcsr, orig_mxcsr;
  _FPU_GETSSECW (orig_mxcsr);
  mxcsr = orig_mxcsr | (exceptions << 7);
  if (mxcsr != orig_mxcsr)
    _FPU_SETSSECW (mxcsr);

  unsigned int trapbits = 0x3f & ~(orig_mxcsr >> 7);

#  else

  /* Disable the traps in the 387 unit.  */
  unsigned short fctrl, orig_fctrl;
  _FPU_GETCW (orig_fctrl);
  fctrl = orig_fctrl | exceptions;
  if (fctrl != orig_fctrl)
    _FPU_SETCW (fctrl);

  if (CPU_HAS_SSE ())
    {
      /* Disable the traps in the SSE unit as well.  */
      unsigned int mxcsr, orig_mxcsr;
      _FPU_GETSSECW (orig_mxcsr);
      mxcsr = orig_mxcsr | (exceptions << 7);
      if (mxcsr != orig_mxcsr)
        _FPU_SETSSECW (mxcsr);
    }

  unsigned int trapbits = 0x3f & ~orig_fctrl;

#  endif

  return x86hardware_to_exceptions (trapbits);
}

int
fegetexcept (void)
{
#  if defined _MSC_VER
  /* Look at the trap bits in the SSE unit.  */
  unsigned int mxcsr;
  _FPU_GETSSECW (mxcsr);
  unsigned int trapbits = 0x3f & ~(mxcsr >> 7);
#  else
  /* Look at the trap bits in the 387 unit.  */
  unsigned short fctrl;
  _FPU_GETCW (fctrl);
  unsigned int trapbits = 0x3f & ~fctrl;
#  endif

  return x86hardware_to_exceptions (trapbits);
}

# elif defined __aarch64__ /* arm64 */

int
feenableexcept (int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;

  unsigned long fpcr, orig_fpcr;
  _FPU_GETCW (orig_fpcr);
  fpcr = orig_fpcr | (exceptions << 8);
  if (fpcr != orig_fpcr)
    {
      _FPU_SETCW (fpcr);
      /* Test whether fpcr was actually changed as desired.  */
      unsigned long new_fpcr;
      _FPU_GETCW (new_fpcr);
      if (new_fpcr != fpcr)
        return -1;
    }

  return FE_ALL_EXCEPT & (orig_fpcr >> 8);
}

int
fedisableexcept (int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;

  unsigned long fpcr, orig_fpcr;
  _FPU_GETCW (orig_fpcr);
  fpcr = orig_fpcr & ~(exceptions << 8);
  if (fpcr != orig_fpcr)
    {
      _FPU_SETCW (fpcr);
      /* Testing whether fpcr was actually changed as desired is not needed
         here, since we only cleared some bits.  */
    }

  return FE_ALL_EXCEPT & (orig_fpcr >> 8);
}

int
fegetexcept (void)
{
  unsigned long fpcr;
  _FPU_GETCW (fpcr);
  return FE_ALL_EXCEPT & (fpcr >> 8);
}

# elif defined __arm__

int
feenableexcept (int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;

#  ifdef __SOFTFP__
  if (exceptions != 0)
    return -1;
  return 0;
#  else
  unsigned int fpscr, orig_fpscr;
  _FPU_GETCW (orig_fpscr);
  fpscr = orig_fpscr | (exceptions << 8);
  if (fpscr != orig_fpscr)
    {
      _FPU_SETCW (fpscr);
      /* Test whether fpscr was actually changed as desired.  */
      unsigned int new_fpscr;
      _FPU_GETCW (new_fpscr);
      if (new_fpscr != fpscr)
        return -1;
    }

  return FE_ALL_EXCEPT & (orig_fpscr >> 8);
#  endif
}

int
fedisableexcept (int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;

#  ifdef __SOFTFP__
  return 0;
#  else
  unsigned int fpscr, orig_fpscr;
  _FPU_GETCW (orig_fpscr);
  fpscr = orig_fpscr & ~(exceptions << 8);
  if (fpscr != orig_fpscr)
    {
      _FPU_SETCW (fpscr);
      /* Testing whether fpscr was actually changed as desired is not needed
         here, since we only cleared some bits.  */
    }

  return FE_ALL_EXCEPT & (orig_fpscr >> 8);
#  endif
}

int
fegetexcept (void)
{
#  ifdef __SOFTFP__
  return 0;
#  else
  unsigned int fpscr;
  _FPU_GETCW (fpscr);
  return FE_ALL_EXCEPT & (fpscr >> 8);
#  endif
}

# elif defined __alpha

int
feenableexcept (int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;

  unsigned long swcr, orig_swcr;
  orig_swcr = __ieee_get_fp_control ();
  swcr = orig_swcr | ((unsigned long) exceptions >> 16);
  if (swcr != orig_swcr)
    __ieee_set_fp_control (swcr);

  return FE_ALL_EXCEPT & (orig_swcr << 16);
}

int
fedisableexcept (int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;

  unsigned long swcr, orig_swcr;
  orig_swcr = __ieee_get_fp_control ();
  swcr = orig_swcr & ~((unsigned long) exceptions >> 16);
  if (swcr != orig_swcr)
    __ieee_set_fp_control (swcr);

  return FE_ALL_EXCEPT & (orig_swcr << 16);
}

int
fegetexcept (void)
{
  unsigned long swcr = __ieee_get_fp_control ();
  return FE_ALL_EXCEPT & (swcr << 16);
}

# elif defined __hppa

int
feenableexcept (int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;

  union { unsigned long long fpreg; unsigned int halfreg[2]; } s;
  /* Get the current status word. */
  __asm__ __volatile__ ("fstd %%fr0,0(%1)" : "=m" (s.fpreg) : "r" (&s.fpreg) : "%r0");
  unsigned int old_halfreg0 = s.halfreg[0];
  /* Set all the relevant bits. */
  s.halfreg[0] |= (unsigned int) exceptions;
  if (s.halfreg[0] != old_halfreg0)
    {
      /* Store the new status word.  */
      __asm__ __volatile__ ("fldd 0(%0),%%fr0" : : "r" (&s.fpreg), "m" (s.fpreg) : "%r0");
    }

  return FE_ALL_EXCEPT & old_halfreg0;
}

int
fedisableexcept (int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;

  union { unsigned long long fpreg; unsigned int halfreg[2]; } s;
  /* Get the current status word. */
  __asm__ __volatile__ ("fstd %%fr0,0(%1)" : "=m" (s.fpreg) : "r" (&s.fpreg) : "%r0");
  unsigned int old_halfreg0 = s.halfreg[0];
  /* Clear all the relevant bits. */
  s.halfreg[0] &= ~ (unsigned int) exceptions;
  if (s.halfreg[0] != old_halfreg0)
    {
      /* Store the new status word.  */
      __asm__ __volatile__ ("fldd 0(%0),%%fr0" : : "r" (&s.fpreg), "m" (s.fpreg) : "%r0");
    }

  return FE_ALL_EXCEPT & old_halfreg0;
}

int
fegetexcept (void)
{
  union { unsigned long long fpreg; unsigned int halfreg[2]; } s;
  /* Get the current status word. */
  __asm__ __volatile__ ("fstd %%fr0,0(%1)" : "=m" (s.fpreg) : "r" (&s.fpreg) : "%r0");

  return FE_ALL_EXCEPT & s.halfreg[0];
}

# elif defined __ia64__

int
feenableexcept (int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;

  unsigned long fpsr, orig_fpsr;
  _FPU_GETCW (orig_fpsr);
  fpsr = orig_fpsr & ~ (unsigned long) exceptions;
  if (fpsr != orig_fpsr)
    _FPU_SETCW (fpsr);

  return FE_ALL_EXCEPT & ~orig_fpsr;
}

int
fedisableexcept (int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;

  unsigned long fpsr, orig_fpsr;
  _FPU_GETCW (orig_fpsr);
  fpsr = orig_fpsr | (unsigned long) exceptions;
  if (fpsr != orig_fpsr)
    _FPU_SETCW (fpsr);

  return FE_ALL_EXCEPT & ~orig_fpsr;
}

int
fegetexcept (void)
{
  unsigned long fpsr;
  _FPU_GETCW (fpsr);
  return FE_ALL_EXCEPT & ~fpsr;
}

# elif defined __m68k__

int
feenableexcept (int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;

  unsigned int fpcr, orig_fpcr;
  _FPU_GETCW (orig_fpcr);
  fpcr = orig_fpcr | ((exceptions << 6) | (exceptions & FE_INVALID ? 0xc000 : 0));
  if (fpcr != orig_fpcr)
    _FPU_SETCW (fpcr);

  return FE_ALL_EXCEPT & (orig_fpcr >> 6);
}

int
fedisableexcept (int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;

  unsigned int fpcr, orig_fpcr;
  _FPU_GETCW (orig_fpcr);
  fpcr = orig_fpcr & ~ ((exceptions << 6) | (exceptions & FE_INVALID ? 0xc000 : 0));
  if (fpcr != orig_fpcr)
    _FPU_SETCW (fpcr);

  return FE_ALL_EXCEPT & (orig_fpcr >> 6);
}

int
fegetexcept (void)
{
  unsigned int fpcr;
  _FPU_GETCW (fpcr);
  return FE_ALL_EXCEPT & (fpcr >> 6);
}

# elif defined __mips__

int
feenableexcept (int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;

  unsigned int fcsr, orig_fcsr;
  _FPU_GETCW (orig_fcsr);
  fcsr = orig_fcsr | (exceptions << 5);
  if (fcsr != orig_fcsr)
    _FPU_SETCW (fcsr);

  return FE_ALL_EXCEPT & (orig_fcsr >> 5);
}

int
fedisableexcept (int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;

  unsigned int fcsr, orig_fcsr;
  _FPU_GETCW (orig_fcsr);
  fcsr = orig_fcsr & ~ (exceptions << 5);
  if (fcsr != orig_fcsr)
    _FPU_SETCW (fcsr);

  return FE_ALL_EXCEPT & (orig_fcsr >> 5);
}

int
fegetexcept (void)
{
  unsigned int fcsr;
  _FPU_GETCW (fcsr);
  return FE_ALL_EXCEPT & (fcsr >> 5);
}

# elif defined __loongarch__

int
feenableexcept (int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;

  unsigned int fcsr, orig_fcsr;
  _FPU_GETCW (orig_fcsr);
  fcsr = orig_fcsr | (exceptions >> 16);
  if (fcsr != orig_fcsr)
    _FPU_SETCW (fcsr);

  return FE_ALL_EXCEPT & (orig_fcsr << 16);
}

int
fedisableexcept (int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;

  unsigned int fcsr, orig_fcsr;
  _FPU_GETCW (orig_fcsr);
  fcsr = orig_fcsr & ~ (exceptions >> 16);
  if (fcsr != orig_fcsr)
    _FPU_SETCW (fcsr);

  return FE_ALL_EXCEPT & (orig_fcsr << 16);
}

int
fegetexcept (void)
{
  unsigned int fcsr;
  _FPU_GETCW (fcsr);
  return FE_ALL_EXCEPT & (fcsr << 16);
}

# elif defined _ARCH_PPC

#  if defined __linux__
#   include <sys/prctl.h>
#  elif defined _AIX
#   include <fptrap.h>
#  endif

int
feenableexcept (int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;

  union { unsigned long long u; double f; } memenv, orig_memenv;
  _FPU_GETCW_AS_DOUBLE (memenv.f);
  orig_memenv = memenv;

  memenv.u |= ((unsigned int) exceptions >> 22);

  if (!(memenv.u == orig_memenv.u))
    {
      if ((orig_memenv.u & 0x000000f8) == 0 && (memenv.u & 0x000000f8) != 0)
        {
          /* Put the thread into precise trapping mode.  */
#  if defined __linux__ || defined __NetBSD__
          prctl (PR_SET_FPEXC, PR_FP_EXC_PRECISE);
#  elif defined _AIX
          /* Documentation: <https://www.ibm.com/docs/en/aix/7.3?topic=f-fp-trap-subroutine>  */
          fp_trap (FP_TRAP_SYNC);
#  endif
        }

      _FPU_SETCW_AS_DOUBLE (memenv.f);
    }

  return FE_ALL_EXCEPT & ((unsigned int) orig_memenv.u << 22);
}

int
fedisableexcept (int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;

  union { unsigned long long u; double f; } memenv, orig_memenv;
  _FPU_GETCW_AS_DOUBLE (memenv.f);
  orig_memenv = memenv;

  memenv.u &= ~ ((unsigned int) exceptions >> 22);

  if (!(memenv.u == orig_memenv.u))
    {
      _FPU_SETCW_AS_DOUBLE (memenv.f);

      if ((orig_memenv.u & 0x000000f8) != 0 && (memenv.u & 0x000000f8) == 0)
        {
          /* Put the thread into no-trapping mode.  */
#  if defined __linux__ || defined __NetBSD__
          prctl (PR_SET_FPEXC, PR_FP_EXC_DISABLED);
#  elif defined _AIX
          /* Documentation: <https://www.ibm.com/docs/en/aix/7.3?topic=f-fp-trap-subroutine>  */
          fp_trap (FP_TRAP_OFF);
#  endif
        }
    }

  return FE_ALL_EXCEPT & ((unsigned int) orig_memenv.u << 22);
}

int
fegetexcept (void)
{
  union { unsigned long long u; double f; } memenv;
  _FPU_GETCW_AS_DOUBLE (memenv.f);

  return FE_ALL_EXCEPT & ((unsigned int) memenv.u << 22);
}

# elif defined __riscv

int
feenableexcept (int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;

  if (exceptions != 0)
    return -1;
  return 0;
}

int
fedisableexcept (int exceptions)
{
  return 0;
}

int
fegetexcept (void)
{
  return 0;
}

# elif defined __s390__ || defined __s390x__

int
feenableexcept (int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;

  unsigned int fpc, orig_fpc;
  _FPU_GETCW (orig_fpc);
#  if FE_INEXACT == 8 /* glibc compatible FE_* values */
  fpc = orig_fpc | ((unsigned int) exceptions << 24);
#  else /* musl libc compatible FE_* values */
  fpc = orig_fpc | ((unsigned int) exceptions << 8);
#  endif
  if (fpc != orig_fpc)
    _FPU_SETCW (fpc);

#  if FE_INEXACT == 8 /* glibc compatible FE_* values */
  return FE_ALL_EXCEPT & (orig_fpc >> 24);
#  else /* musl libc compatible FE_* values */
  return FE_ALL_EXCEPT & (orig_fpc >> 8);
#  endif
}

int
fedisableexcept (int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;

  unsigned int fpc, orig_fpc;
  _FPU_GETCW (orig_fpc);
#  if FE_INEXACT == 8 /* glibc compatible FE_* values */
  fpc = orig_fpc & ~((unsigned int) exceptions << 24);
#  else /* musl libc compatible FE_* values */
  fpc = orig_fpc & ~((unsigned int) exceptions << 8);
#  endif
  if (fpc != orig_fpc)
    _FPU_SETCW (fpc);

#  if FE_INEXACT == 8 /* glibc compatible FE_* values */
  return FE_ALL_EXCEPT & (orig_fpc >> 24);
#  else /* musl libc compatible FE_* values */
  return FE_ALL_EXCEPT & (orig_fpc >> 8);
#  endif
}

int
fegetexcept (void)
{
  unsigned int fpc;
  _FPU_GETCW (fpc);
#  if FE_INEXACT == 8 /* glibc compatible FE_* values */
  return FE_ALL_EXCEPT & (fpc >> 24);
#  else /* musl libc compatible FE_* values */
  return FE_ALL_EXCEPT & (fpc >> 8);
#  endif
}

# elif defined __sh__

int
feenableexcept (int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;

  unsigned int fpscr, orig_fpscr;
  _FPU_GETCW (orig_fpscr);
  fpscr = orig_fpscr | ((unsigned int) exceptions << 5);
  if (fpscr != orig_fpscr)
    _FPU_SETCW (fpscr);

  return FE_ALL_EXCEPT & (orig_fpscr >> 5);
}

int
fedisableexcept (int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;

  unsigned int fpscr, orig_fpscr;
  _FPU_GETCW (orig_fpscr);
  fpscr = orig_fpscr & ~ ((unsigned int) exceptions << 5);
  if (fpscr != orig_fpscr)
    _FPU_SETCW (fpscr);

  return FE_ALL_EXCEPT & (orig_fpscr >> 5);
}

int
fegetexcept (void)
{
  unsigned int fpscr;
  _FPU_GETCW (fpscr);
  return FE_ALL_EXCEPT & (fpscr >> 5);
}

# elif defined __sparc

int
feenableexcept (int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;

  unsigned long fsr, orig_fsr;
  _FPU_GETCW (orig_fsr);
#  if FE_INEXACT == 32 /* glibc compatible FE_* values */
  fsr = orig_fsr | (exceptions << 18);
#  else /* Solaris compatible FE_* values */
  fsr = orig_fsr | (exceptions << 23);
#  endif
  if (fsr != orig_fsr)
    _FPU_SETCW (fsr);

#  if FE_INEXACT == 32 /* glibc compatible FE_* values */
  return FE_ALL_EXCEPT & (orig_fsr >> 18);
#  else /* Solaris compatible FE_* values */
  return FE_ALL_EXCEPT & (orig_fsr >> 23);
#  endif
}

int
fedisableexcept (int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;

  unsigned long fsr, orig_fsr;
  _FPU_GETCW (orig_fsr);
#  if FE_INEXACT == 32 /* glibc compatible FE_* values */
  fsr = orig_fsr & ~(exceptions << 18);
#  else /* Solaris compatible FE_* values */
  fsr = orig_fsr & ~(exceptions << 23);
#  endif
  if (fsr != orig_fsr)
    _FPU_SETCW (fsr);

#  if FE_INEXACT == 32 /* glibc compatible FE_* values */
  return FE_ALL_EXCEPT & (orig_fsr >> 18);
#  else /* Solaris compatible FE_* values */
  return FE_ALL_EXCEPT & (orig_fsr >> 23);
#  endif
}

int
fegetexcept (void)
{
  unsigned long fsr;
  _FPU_GETCW (fsr);
#  if FE_INEXACT == 32 /* glibc compatible FE_* values */
  return FE_ALL_EXCEPT & (fsr >> 18);
#  else /* Solaris compatible FE_* values */
  return FE_ALL_EXCEPT & (fsr >> 23);
#  endif
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

# if HAVE_FPSETMASK
/* FreeBSD ≥ 3.1, NetBSD ≥ 1.1, OpenBSD, Solaris, Minix ≥ 3.2.  */

/* Get fpgetmask, fpsetmask.  */
#  include <ieeefp.h>

/* The type is called 'fp_except_t' on FreeBSD, but 'fp_except' on
   all other systems.  */
#  if !defined __FreeBSD__
#   define fp_except_t fp_except
#  endif

static fp_except_t
exceptions_to_mask (int exceptions)
{
  fp_except_t m = 0;
  if (exceptions & FE_INVALID)
    m |= FP_X_INV;
  if (exceptions & FE_DIVBYZERO)
    m |= FP_X_DZ;
  if (exceptions & FE_OVERFLOW)
    m |= FP_X_OFL;
  if (exceptions & FE_UNDERFLOW)
    m |= FP_X_UFL;
  if (exceptions & FE_INEXACT)
    m |= FP_X_IMP;
  return m;
}

static int
mask_to_exceptions (fp_except_t m)
{
  int exceptions = 0;
  if (m & FP_X_INV)
    exceptions |= FE_INVALID;
  if (m & FP_X_DZ)
    exceptions |= FE_DIVBYZERO;
  if (m & FP_X_OFL)
    exceptions |= FE_OVERFLOW;
  if (m & FP_X_UFL)
    exceptions |= FE_UNDERFLOW;
  if (m & FP_X_IMP)
    exceptions |= FE_INEXACT;
  return exceptions;
}

int
feenableexcept (int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;

  fp_except_t trapbits = fpgetmask ();
  fpsetmask (trapbits | exceptions_to_mask (exceptions));
  return mask_to_exceptions (trapbits);
}

int
fedisableexcept (int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;

  fp_except_t trapbits = fpgetmask ();
  fpsetmask (trapbits & ~ exceptions_to_mask (exceptions));
  return mask_to_exceptions (trapbits);
}

int
fegetexcept (void)
{
  fp_except_t trapbits = fpgetmask ();
  return mask_to_exceptions (trapbits);
}

# elif defined _AIX && defined _ARCH_PPC /* AIX */

#  include <fptrap.h>
/* Documentation:
   <https://www.ibm.com/docs/en/aix/7.3?topic=f-fp-any-enable-fp-is-enabled-fp-enable-all-fp-enable-fp-disable-all-fp-disable-subroutine>
   <https://www.ibm.com/docs/en/aix/7.3?topic=f-fp-trap-subroutine>  */

/* Convert from an 'int exceptions' to an fptrap_t.  */
#  if 0 /* Unoptimized */
#   define exceptions_to_fptrap(exceptions) \
      (  ((exceptions) & FE_INVALID   ? TRP_INVALID     : 0) \
       | ((exceptions) & FE_DIVBYZERO ? TRP_DIV_BY_ZERO : 0) \
       | ((exceptions) & FE_OVERFLOW  ? TRP_OVERFLOW    : 0) \
       | ((exceptions) & FE_UNDERFLOW ? TRP_UNDERFLOW   : 0) \
       | ((exceptions) & FE_INEXACT   ? TRP_INEXACT     : 0))
#  else /* Optimized */
#   define exceptions_to_fptrap(exceptions) \
      (((unsigned int) (exceptions) & FE_ALL_EXCEPT) >> 22)
#  endif

int
feenableexcept (int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;

  int prev_enabled = fegetexcept ();
  if (exceptions != 0)
    {
      fp_enable (exceptions_to_fptrap (exceptions));
      if (prev_enabled == 0 /* && fegetexcept () != 0 */)
        /* Enable precise trapping mode.  */
        fp_trap (FP_TRAP_SYNC);
    }
  return prev_enabled;
}

int
fedisableexcept (int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;

  int prev_enabled = fegetexcept ();
  if (exceptions != 0)
    {
      fp_disable (exceptions_to_fptrap (exceptions));
      if (prev_enabled != 0 && fegetexcept () == 0)
        /* Disable precise trapping mode.  */
        fp_trap (FP_TRAP_OFF);
    }
  return prev_enabled;
}

int
fegetexcept (void)
{
  return fegetexcept_impl ();
}

# elif HAVE_FESETTRAPENABLE
/* HP-UX, QNX */

int
feenableexcept (int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;

  int prev_enabled = fegettrapenable ();
  if (exceptions != 0)
    fesettrapenable (prev_enabled | exceptions);
  return prev_enabled;
}

int
fedisableexcept (int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;

  int prev_enabled = fegettrapenable ();
  if (exceptions != 0)
    fesettrapenable (prev_enabled & ~exceptions);
  return prev_enabled;
}

int
fegetexcept (void)
{
  return fegettrapenable ();
}

# else

#  define NEED_FALLBACK 1

# endif

#endif

#if NEED_FALLBACK

/* A dummy fallback.  */

int
feenableexcept (int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;
  if (exceptions != 0)
    return -1;
  return 0;
}

int
fedisableexcept (int exceptions)
{
  return 0;
}

int
fegetexcept (void)
{
  return 0;
}

#endif
