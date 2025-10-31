/* Functions for controlling the floating-point rounding direction.
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

/* Based on glibc/sysdeps/<cpu>/{fegetround.c,fesetround.c}
   together with glibc/sysdeps/<cpu>/{fpu_control.h,fenv_private.h,fenv_libc.h}.  */

#include <config.h>

/* Specification.  */
#include <fenv.h>

#include "fenv-private.h"

#if defined __GNUC__ || defined __clang__ || defined _MSC_VER

# if (defined __x86_64__ || defined _M_X64) || (defined __i386 || defined _M_IX86)

int
fegetround (void)
{
#  ifdef _MSC_VER
  /* Use the rounding direction from the SSE unit.  */
  unsigned int mxcsr;
  _FPU_GETSSECW (mxcsr);
  unsigned int fctrl = (mxcsr >> 3) & 0x0C00;
#  else
  /* Use the rounding direction from the control word of the 387 unit, the
     so-called fctrl register.
     The rounding direction of the SSE unit, in the mxcsr register, is expected
     to be in sync with that.  */
  unsigned short fctrl;
  _FPU_GETCW (fctrl);
#  endif
#  if defined _MSC_VER || (defined __MINGW32__ && FE_INVALID != 0x01)
  /* The MSVC header files have different values for the rounding directions
     than all the other platforms, and they even changed between MSVC 14 and
     MSVC 14.30 (!).  Map
       0x0000 -> FE_TONEAREST = 0
       0x0400 -> FE_DOWNWARD
       0x0800 -> FE_UPWARD
       0x0C00 -> FE_TOWARDZERO = FE_DOWNWARD | FE_UPWARD */
  return (fctrl & 0x0800 ? FE_UPWARD : 0) | (fctrl & 0x0400 ? FE_DOWNWARD : 0);
#  else
  return fctrl & 0x0C00;
#  endif
}

int
fesetround (int rounding_direction)
{
#  if defined _MSC_VER || (defined __MINGW32__ && FE_INVALID != 0x01)
  /* The MSVC header files have different values for the rounding directions
     than all the other platforms.  */
  if ((rounding_direction & ~0x0300) != 0)
    return -1;
  /* The MSVC header files have different values for the rounding directions
     than all the other platforms, and they even changed between MSVC 14 and
     MSVC 14.30 (!).  Map
     FE_TONEAREST = 0                        -> 0x0000
     FE_DOWNWARD                             -> 0x0400
     FE_UPWARD                               -> 0x0800
     FE_TOWARDZERO = FE_DOWNWARD | FE_UPWARD -> 0x0C00  */
  rounding_direction =
    (rounding_direction & FE_UPWARD ? 0x0800 : 0)
    | (rounding_direction & FE_DOWNWARD ? 0x0400 : 0);
#  else
  if ((rounding_direction & ~0x0C00) != 0)
    return -1;
#  endif

#  ifdef _MSC_VER
  /* Set it in the SSE unit.  */
  unsigned int mxcsr, orig_mxcsr;
  _FPU_GETSSECW (orig_mxcsr);
  mxcsr = (orig_mxcsr & ~(0x0C00 << 3)) | (rounding_direction << 3);
  if (mxcsr != orig_mxcsr)
    _FPU_SETSSECW (mxcsr);
#  else
  /* Set it in the 387 unit.  */
  unsigned short fctrl, orig_fctrl;
  _FPU_GETCW (orig_fctrl);
  fctrl = (orig_fctrl & ~0x0C00) | rounding_direction;
  if (fctrl != orig_fctrl)
    _FPU_SETCW (fctrl);

  if (CPU_HAS_SSE ())
    {
      /* Set it in the SSE unit as well.  */
      unsigned int mxcsr, orig_mxcsr;
      _FPU_GETSSECW (orig_mxcsr);
      mxcsr = (orig_mxcsr & ~(0x0C00 << 3)) | (rounding_direction << 3);
      if (mxcsr != orig_mxcsr)
        _FPU_SETSSECW (mxcsr);
    }
#  endif

  return 0;
}

# elif defined __aarch64__ /* arm64 */

int
fegetround (void)
{
  unsigned long fpcr;
  _FPU_GETCW (fpcr);
  return (fpcr & 0x00C00000UL)
#  if FE_TOWARDZERO == 3 /* FreeBSD compatible FE_* values */
         >> 22
#  endif
         ;
}

int
fesetround (int rounding_direction)
{
#  if FE_TOWARDZERO == 3 /* FreeBSD compatible FE_* values */
  if ((rounding_direction & ~3) != 0)
    return -1;
  rounding_direction = rounding_direction << 22;
#  else /* glibc compatible FE_* values */
  if ((rounding_direction & ~0x00C00000UL) != 0)
    return -1;
#  endif
  unsigned long fpcr, orig_fpcr;
  _FPU_GETCW (orig_fpcr);
  fpcr = (orig_fpcr & ~0x00C00000UL) | rounding_direction;
  if (fpcr != orig_fpcr)
    _FPU_SETCW (fpcr);
  return 0;
}

# elif defined __arm__

int
fegetround (void)
{
#  ifdef __SOFTFP__
  return FE_TONEAREST;
#  else
  unsigned int fpscr;
  _FPU_GETCW (fpscr);
  return fpscr & 0x00C00000U;
#  endif
}

int
fesetround (int rounding_direction)
{
#  ifdef __SOFTFP__
  if (rounding_direction != FE_TONEAREST)
    return -1;
#  else
  if ((rounding_direction & ~0x00C00000U) != 0)
    return -1;
  unsigned int fpscr, orig_fpscr;
  _FPU_GETCW (orig_fpscr);
  fpscr = (orig_fpscr & ~0x00C00000U) | rounding_direction;
  if (fpscr != orig_fpscr)
    _FPU_SETCW (fpscr);
#  endif
  return 0;
}

# elif defined __alpha

int
fegetround (void)
{
  unsigned long fpcr;
  _FPU_GETCW (fpcr);
  return (fpcr >> 58) & 0x3UL;
}

int
fesetround (int rounding_direction)
{
  if ((rounding_direction & ~0x3UL) != 0)
    return -1;
  unsigned long fpcr, orig_fpcr;
  _FPU_GETCW (orig_fpcr);
  fpcr = (orig_fpcr & ~(0x3UL << 58))
         | ((unsigned long) rounding_direction << 58);
  if (fpcr != orig_fpcr)
    _FPU_SETCW (fpcr);
  return 0;
}

# elif defined __hppa

int
fegetround (void)
{
  unsigned int fpstatus;
  _FPU_GETCW (fpstatus);
  return fpstatus & 0x00000600U;
}

int
fesetround (int rounding_direction)
{
  if ((rounding_direction & ~0x00000600U) != 0)
    return -1;
  unsigned int fpstatus, orig_fpstatus;
  _FPU_GETCW (orig_fpstatus);
  fpstatus = (orig_fpstatus & ~0x00000600U) | rounding_direction;
  if (fpstatus != orig_fpstatus)
    _FPU_SETCW (fpstatus);
  return 0;
}

# elif defined __ia64__

int
fegetround (void)
{
  unsigned long fpsr;
  _FPU_GETCW (fpsr);
  return (fpsr >> 10) & 0x3UL;
}

int
fesetround (int rounding_direction)
{
  if ((rounding_direction & ~0x3UL) != 0)
    return -1;
  unsigned long fpsr, orig_fpsr;
  _FPU_GETCW (orig_fpsr);
  fpsr = (orig_fpsr & ~(0x3UL << 10))
         | ((unsigned long) rounding_direction << 10);
  if (fpsr != orig_fpsr)
    _FPU_SETCW (fpsr);
  return 0;
}

# elif defined __m68k__

int
fegetround (void)
{
  unsigned int fpcr;
  _FPU_GETCW (fpcr);
  return fpcr & 0x30U;
}

int
fesetround (int rounding_direction)
{
  if ((rounding_direction & ~0x30U) != 0)
    return -1;
  unsigned int fpcr, orig_fpcr;
  _FPU_GETCW (orig_fpcr);
  fpcr = (orig_fpcr & ~0x30U) | rounding_direction;
  if (fpcr != orig_fpcr)
    _FPU_SETCW (fpcr);
  return 0;
}

# elif defined __mips__

int
fegetround (void)
{
  unsigned int fcsr;
  _FPU_GETCW (fcsr);
  return fcsr & 0x3U;
}

int
fesetround (int rounding_direction)
{
  if ((rounding_direction & ~0x3U) != 0)
    return -1;
  unsigned int fcsr, orig_fcsr;
  _FPU_GETCW (orig_fcsr);
  fcsr = (orig_fcsr & ~0x3U) | rounding_direction;
  if (fcsr != orig_fcsr)
    _FPU_SETCW (fcsr);
  return 0;
}

# elif defined __loongarch__

int
fegetround (void)
{
  unsigned int fcsr;
  _FPU_GETCW (fcsr);
  return fcsr & 0x300U;
}

int
fesetround (int rounding_direction)
{
  if ((rounding_direction & ~0x300U) != 0)
    return -1;
  unsigned int fcsr, orig_fcsr;
  _FPU_GETCW (orig_fcsr);
  fcsr = (orig_fcsr & ~0x300U) | rounding_direction;
  if (fcsr != orig_fcsr)
    _FPU_SETCW (fcsr);
  return 0;
}

# elif defined _ARCH_PPC

/* The AIX header files have different values for the rounding directions
   than all the other platforms: The values 0 and 1 are swapped.
   (They probably did this in order to have a trivial FLT_ROUNDS macro, cf.
   <https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/float.h.html>.)
   Define some handy macros for conversion.  */
#  ifdef _AIX
#   define fe_to_hardware(x) ((x) ^ ((x) < 2))
#   define hardware_to_fe(x) ((x) ^ ((x) < 2))
#  else
#   define fe_to_hardware(x) (x)
#   define hardware_to_fe(x) (x)
#  endif

int
fegetround (void)
{
#  if 1
  unsigned int result;
  __asm__ __volatile__ ("mcrfs 7,7 ; mfcr %0" : "=r" (result) : : "cr7");
  return hardware_to_fe (result & 3);
#  else
  union { unsigned long long u; double f; } memenv;
  _FPU_GETCW_AS_DOUBLE (memenv.f);
  return hardware_to_fe (memenv.u & 3);
#  endif
}

int
fesetround (int rounding_direction)
{
  if ((rounding_direction & ~0x3U) != 0)
    return -1;
#  ifdef _AIX
  rounding_direction = fe_to_hardware (rounding_direction);
#  endif
  if (rounding_direction & 2)
    __asm__ __volatile__ ("mtfsb1 30");
  else
    __asm__ __volatile__ ("mtfsb0 30");
  if (rounding_direction & 1)
    __asm__ __volatile__ ("mtfsb1 31");
  else
    __asm__ __volatile__ ("mtfsb0 31");
  return 0;
}

# elif defined __riscv

int
fegetround (void)
{
  int rounding_direction;
  __asm__ __volatile__ ("frrm %0" : "=r" (rounding_direction));
#  if FE_UPWARD == 0x60 /* FreeBSD compatible FE_* values */
  return rounding_direction << 5;
#  else
  return rounding_direction;
#  endif
}

int
fesetround (int rounding_direction)
{
#  if FE_UPWARD == 0x60 /* FreeBSD compatible FE_* values */
  if ((rounding_direction & ~0x60) != 0)
    return -1;
  rounding_direction = rounding_direction >> 5;
#  else
  if ((rounding_direction & ~3) != 0)
    return -1;
#  endif
  __asm__ __volatile__ ("fsrm %z0" : : "rJ" (rounding_direction));
  return 0;
}

# elif defined __s390__ || defined __s390x__

int
fegetround (void)
{
  unsigned int fpc;
  _FPU_GETCW (fpc);
  return fpc & 0x3U;
}

int
fesetround (int rounding_direction)
{
  if ((rounding_direction & ~0x3U) != 0)
    return -1;
#  if 1
  __asm__ __volatile__ ("srnm 0(%0)" : : "a" (rounding_direction));
#  else
  unsigned int fpc, orig_fpc;
  _FPU_GETCW (orig_fpc);
  fpc = (orig_fpc & ~0x3U) | rounding_direction;
  if (fpc != orig_fpc)
    _FPU_SETCW (fpc);
#  endif
  return 0;
}

# elif defined __sh__

int
fegetround (void)
{
  unsigned int fpscr;
  _FPU_GETCW (fpscr);
  return fpscr & 0x1U;
}

int
fesetround (int rounding_direction)
{
  if ((rounding_direction & ~0x1U) != 0)
    return -1;
  unsigned int fpscr, orig_fpscr;
  _FPU_GETCW (orig_fpscr);
  fpscr = (orig_fpscr & ~0x1U) | rounding_direction;
  if (fpscr != orig_fpscr)
    _FPU_SETCW (fpscr);
  return 0;
}

# elif defined __sparc

int
fegetround (void)
{
  unsigned long fsr;
  _FPU_GETCW (fsr);
  return (fsr & 0xC0000000UL)
#  if FE_DOWNWARD == 3 /* FreeBSD compatible FE_* values */
         >> 30
#  endif
         ;
}

int
fesetround (int rounding_direction)
{
#  if FE_DOWNWARD == 3 /* FreeBSD compatible FE_* values */
  if ((rounding_direction & ~3) != 0)
    return -1;
  rounding_direction = (unsigned int) rounding_direction << 30;
#  else /* glibc compatible FE_* values */
  if (((unsigned int) rounding_direction & ~0xC0000000UL) != 0)
    return -1;
#  endif
  unsigned long fsr, orig_fsr;
  _FPU_GETCW (orig_fsr);
  fsr = (orig_fsr & ~0xC0000000UL) | (unsigned int) rounding_direction;
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

# if HAVE_FPSETROUND
/* FreeBSD ≥ 3.1, NetBSD ≥ 1.1, OpenBSD, Solaris, Minix ≥ 3.2.  */

/* Get fpgetround, fpsetround.  */
#  include <ieeefp.h>

int
fegetround (void)
{
  return fpgetround ();
}

int
fesetround (int rounding_direction)
{
  fpsetround (rounding_direction);
  return 0;
}

# elif defined _AIX /* AIX */

/* Get fp_read_rnd, fp_swap_rnd.  */
#  include <float.h>

/* Documentation:
   <https://www.ibm.com/docs/en/aix/7.3?topic=f-fp-read-rnd-fp-swap-rnd-subroutine>  */

int
fegetround (void)
{
  return fp_read_rnd ();
}

int
fesetround (int rounding_direction)
{
  fp_swap_rnd (rounding_direction);
  return 0;
}

# else

#  define NEED_FALLBACK 1

# endif

#endif

#if NEED_FALLBACK

/* A dummy fallback.  */

# include <float.h>
# include <stdlib.h>

int
fegetround (void)
{
  /* Cf. <https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/float.h.html> */
  switch (FLT_ROUNDS)
    {
    case 0: return FE_TOWARDZERO;
    case 1: return FE_TONEAREST;
    case 2: return FE_UPWARD;
    case 3: return FE_DOWNWARD;
    default: abort ();
    }
}

int
fesetround (int rounding_direction)
{
  if (rounding_direction != fegetround ())
    return -1;
  return 0;
}

#endif
