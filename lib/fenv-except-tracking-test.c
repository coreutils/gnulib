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

#if defined _AIX && defined _ARCH_PPC /* AIX */

/* On AIX, the register fpscr is augmented with a 32-bit word named fpscrx
   in thread-local storage.  Instead of accessing fpscr, we must access the
   combination.  The function fp_read_flag() does this.  */

# include <float.h>
# include <fpxcp.h>

/* Documentation:
   <https://www.ibm.com/docs/en/aix/7.3?topic=f-fp-clr-flag-fp-set-flag-fp-read-flag-fp-swap-flag-subroutine>  */

int
fetestexcept (int exceptions)
{
  fpflag_t flags = fp_read_flag ();
  return fpflag_to_exceptions (flags) & FE_ALL_EXCEPT & exceptions;
}

#elif defined __GNUC__ || defined __clang__ || defined _MSC_VER

# if (defined __x86_64__ || defined _M_X64) || (defined __i386 || defined _M_IX86)

int
fetestexcept (int exceptions)
{
#  if defined _MSC_VER

  /* Look at the flags in the SSE unit.  */
  unsigned int mxcsr;
  _FPU_GETSSECW (mxcsr);
  return x86hardware_to_exceptions (mxcsr) & FE_ALL_EXCEPT & exceptions;

#  else

  unsigned short fstat;
  _FPU_GETSTAT (fstat);

  unsigned int mxcsr = 0;
  if (CPU_HAS_SSE ())
    {
      /* Look at the flags in the SSE unit as well.  */
      _FPU_GETSSECW (mxcsr);
    }

  return x86hardware_to_exceptions (fstat | mxcsr) & FE_ALL_EXCEPT & exceptions;

#  endif
}

# elif defined __aarch64__ /* arm64 */

int
fetestexcept (int exceptions)
{
  unsigned long fpsr;
  _FPU_GETFPSR (fpsr);
  return fpsr & FE_ALL_EXCEPT & exceptions;
}

# elif defined __arm__

int
fetestexcept (int exceptions)
{
#  ifdef __SOFTFP__
  return 0;
#  else
  unsigned int fpscr;
  _FPU_GETCW (fpscr);
  return fpscr & FE_ALL_EXCEPT & exceptions;
#  endif
}

# elif defined __alpha

int
fetestexcept (int exceptions)
{
  unsigned long swcr = __ieee_get_fp_control ();
  return swcr & FE_ALL_EXCEPT & exceptions;
}

# elif defined __hppa

int
fetestexcept (int exceptions)
{
  union { unsigned long long fpreg; unsigned int halfreg[2]; } s;
  /* Get the current status word. */
  __asm__ __volatile__ ("fstd %%fr0,0(%1)" : "=m" (s.fpreg) : "r" (&s.fpreg) : "%r0");
  return (s.halfreg[0] >> 27) & FE_ALL_EXCEPT & exceptions;
}

# elif defined __ia64__

int
fetestexcept (int exceptions)
{
  unsigned long fpsr;
  _FPU_GETCW (fpsr);
  return (fpsr >> 13) & FE_ALL_EXCEPT & exceptions;
}

# elif defined __m68k__

int
fetestexcept (int exceptions)
{
  unsigned int fpsr;
  _FPU_GETFPSR (fpsr);
  return fpsr & FE_ALL_EXCEPT & exceptions;
}

# elif defined __mips__

int
fetestexcept (int exceptions)
{
  unsigned int fcsr;
  _FPU_GETCW (fcsr);
  return fcsr & FE_ALL_EXCEPT & exceptions;
}

# elif defined __loongarch__

int
fetestexcept (int exceptions)
{
  unsigned int fcsr;
  _FPU_GETCW (fcsr);
  return fcsr & FE_ALL_EXCEPT & exceptions;
}

# elif defined _ARCH_PPC

int
fetestexcept (int exceptions)
{
  union { unsigned long long u; double f; } memenv;
  _FPU_GETCW_AS_DOUBLE (memenv.f);
  return memenv.u & FE_ALL_EXCEPT & exceptions;
}

# elif defined __riscv

int
fetestexcept (int exceptions)
{
  unsigned int flags;
  __asm__ __volatile__ ("frflags %0" : "=r" (flags)); /* same as "csrr %0, fflags" */
  return flags & FE_ALL_EXCEPT & exceptions;
}

# elif defined __s390__ || defined __s390x__

int
fetestexcept (int exceptions)
{
  unsigned int fpc;
  _FPU_GETCW (fpc);
#  if FE_INEXACT == 8 /* glibc compatible FE_* values */
  return ((fpc >> 16) | ((fpc & 0x00000300) == 0 ? fpc >> 8 : 0))
         & FE_ALL_EXCEPT & exceptions;
#  else /* musl libc compatible FE_* values */
  return (fpc | ((fpc & 0x00000300) == 0 ? fpc << 8 : 0))
         & FE_ALL_EXCEPT & exceptions;
#  endif
}

# elif defined __sh__

int
fetestexcept (int exceptions)
{
  unsigned int fpscr;
  _FPU_GETCW (fpscr);
  return fpscr & FE_ALL_EXCEPT & exceptions;
}

# elif defined __sparc

int
fetestexcept (int exceptions)
{
  unsigned long fsr;
  _FPU_GETCW (fsr);
#  if FE_INEXACT == 32 /* glibc compatible FE_* values */
  return fsr & FE_ALL_EXCEPT & exceptions;
#  else /* Solaris compatible FE_* values */
  return (fsr >> 5) & FE_ALL_EXCEPT & exceptions;
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
fetestexcept (int exceptions)
{
  fp_except_t flags = fpgetsticky ();
  return flags & FE_ALL_EXCEPT & exceptions;
}

# else

#  define NEED_FALLBACK 1

# endif

#endif

#if NEED_FALLBACK

/* A dummy fallback.  */

int
fetestexcept (int exceptions)
{
  return 0;
}

#endif
