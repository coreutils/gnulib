/* Functions for saving the floating-point exception status flags.
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

/* Based on glibc/sysdeps/<cpu>/{fgetexcptflg.c,fsetexcptflg.c}
   together with glibc/sysdeps/<cpu>/{fpu_control.h,fenv_private.h,fenv_libc.h}.  */

#include <config.h>

/* Specification.  */
#include <fenv.h>

#include "fenv-private.h"

#if defined __GNUC__ || defined __clang__ || defined _MSC_VER

# if (defined __x86_64__ || defined _M_X64) || (defined __i386 || defined _M_IX86)

/* On most OSes, fexcept_t is binary-equivalent to an 'unsigned short'.
   On NetBSD, OpenBSD, Solaris, Cygwin, MSVC, Android/x86_64, Minix, fexcept_t
   is equivalent to an 'unsigned int'.
   A simple C cast does the necessary conversion.  */

int
fegetexceptflag (fexcept_t *saved_flags, int exceptions)
{
  /* Just like fetestexcept.  */
#  if defined _MSC_VER

  unsigned int mxcsr;
  _FPU_GETSSECW (mxcsr);
  *saved_flags = x86hardware_to_exceptions (mxcsr) & FE_ALL_EXCEPT & exceptions;

#  else

  unsigned short fstat;
  _FPU_GETSTAT (fstat);

  unsigned int mxcsr = 0;
  if (CPU_HAS_SSE ())
    {
      /* Look at the flags in the SSE unit as well.  */
      _FPU_GETSSECW (mxcsr);
    }

  *saved_flags = x86hardware_to_exceptions (fstat | mxcsr)
                 & FE_ALL_EXCEPT & exceptions;
#  endif

  return 0;
}

# elif defined __aarch64__ /* arm64 */

/* On Linux, NetBSD, and Android, fexcept_t is binary-equivalent to
   an 'unsigned int'.
   On macOS, fexcept_t is binary-equivalent to an 'unsigned short'.
   On FreeBSD and OpenBSD, fexcept_t is binary-equivalent to an 'unsigned long'.
   A simple C cast does the necessary conversion.  */

int
fegetexceptflag (fexcept_t *saved_flags, int exceptions)
{
  /* Just like fetestexcept.  */
  unsigned long fpsr;
  _FPU_GETFPSR (fpsr);
  *saved_flags = fpsr & FE_ALL_EXCEPT & exceptions;
  return 0;
}

# elif defined __arm__

/* On all OSes, fexcept_t is binary-equivalent to an 'unsigned int'.  */

int
fegetexceptflag (fexcept_t *saved_flags, int exceptions)
{
  /* Just like fetestexcept.  */
#  ifdef __SOFTFP__
  return -1;
#  else
  unsigned int fpscr;
  _FPU_GETCW (fpscr);
  *saved_flags = fpscr & FE_ALL_EXCEPT & exceptions;
  return 0;
#  endif
}

# elif defined __alpha

/* On all OSes except NetBSD and OpenBSD, fexcept_t is binary-equivalent to
   an 'unsigned long'.
   On NetBSD, it is equivalent to an 'unsigned short'.
   On OpenBSD, it is equivalent to an 'unsigned int'.
   A simple C cast does the necessary conversion.  */

int
fegetexceptflag (fexcept_t *saved_flags, int exceptions)
{
  /* Just like fetestexcept.  */
  unsigned long swcr = __ieee_get_fp_control ();
  *saved_flags = swcr & FE_ALL_EXCEPT & exceptions;
  return 0;
}

# elif defined __hppa

/* On all OSes, fexcept_t is binary-equivalent to an 'unsigned int'.  */

int
fegetexceptflag (fexcept_t *saved_flags, int exceptions)
{
  /* Just like fetestexcept.  */
  union { unsigned long long fpreg; unsigned int halfreg[2]; } s;
  /* Get the current status word. */
  __asm__ __volatile__ ("fstd %%fr0,0(%1)" : "=m" (s.fpreg) : "r" (&s.fpreg) : "%r0");
  *saved_flags = (s.halfreg[0] >> 27) & FE_ALL_EXCEPT & exceptions;
  return 0;
}

# elif defined __ia64__

/* On all OSes except NetBSD, fexcept_t is binary-equivalent to
   an 'unsigned long'.
   On NetBSD, it is equivalent to an 'unsigned short'.
   A simple C cast does the necessary conversion.  */

int
fegetexceptflag (fexcept_t *saved_flags, int exceptions)
{
  /* Just like fetestexcept.  */
  unsigned long fpsr;
  _FPU_GETCW (fpsr);
  *saved_flags = (fpsr >> 13) & FE_ALL_EXCEPT & exceptions;
  return 0;
}

# elif defined __m68k__

/* On all OSes, fexcept_t is binary-equivalent to an 'unsigned int'.  */

int
fegetexceptflag (fexcept_t *saved_flags, int exceptions)
{
  /* Just like fetestexcept.  */
  unsigned int fpsr;
  _FPU_GETFPSR (fpsr);
  *saved_flags = fpsr & FE_ALL_EXCEPT & exceptions;
  return 0;
}

# elif defined __mips__

/* On all OSes except NetBSD and OpenBSD, fexcept_t is binary-equivalent to
   an 'unsigned short'.
   On NetBSD and OpenBSD, it is equivalent to an 'unsigned int'.
   A simple C cast does the necessary conversion.  */

int
fegetexceptflag (fexcept_t *saved_flags, int exceptions)
{
  /* Just like fetestexcept.  */
  unsigned int fcsr;
  _FPU_GETCW (fcsr);
  *saved_flags = fcsr & FE_ALL_EXCEPT & exceptions;
  return 0;
}

# elif defined __loongarch__

/* On all OSes, fexcept_t is binary-equivalent to an 'unsigned int'.  */

int
fegetexceptflag (fexcept_t *saved_flags, int exceptions)
{
  /* Just like fetestexcept.  */
  unsigned int fcsr;
  _FPU_GETCW (fcsr);
  *saved_flags = fcsr & FE_ALL_EXCEPT & exceptions;
  return 0;
}

# elif defined _ARCH_PPC

/* On all OSes, fexcept_t is binary-equivalent to an 'unsigned int'.  */

int
fegetexceptflag (fexcept_t *saved_flags, int exceptions)
{
  /* Just like fetestexcept.  */
  union { unsigned long long u; double f; } memenv;
  _FPU_GETCW_AS_DOUBLE (memenv.f);
  *saved_flags = memenv.u & FE_ALL_EXCEPT & exceptions;
  return 0;
}

# elif defined __riscv

/* On all OSes except FreeBSD, fexcept_t is binary-equivalent to
   an 'unsigned int'.
   On FreeBSD, it is equivalent to an 'unsigned long'.
   A simple C cast does the necessary conversion.  */

int
fegetexceptflag (fexcept_t *saved_flags, int exceptions)
{
  /* Just like fetestexcept.  */
  unsigned int flags;
  __asm__ __volatile__ ("frflags %0" : "=r" (flags)); /* same as "csrr %0, fflags" */
  *saved_flags = flags & FE_ALL_EXCEPT & exceptions;
  return 0;
}

# elif defined __s390__ || defined __s390x__

/* On all OSes, fexcept_t is binary-equivalent to an 'unsigned int'.  */

int
fegetexceptflag (fexcept_t *saved_flags, int exceptions)
{
  /* Just like fetestexcept.  */
  unsigned int fpc;
  _FPU_GETCW (fpc);
#  if FE_INEXACT == 8 /* glibc compatible FE_* values */
  *saved_flags = ((fpc >> 16) | ((fpc & 0x00000300) == 0 ? fpc >> 8 : 0))
                 & FE_ALL_EXCEPT & exceptions;
#  else /* musl libc compatible FE_* values */
  *saved_flags = (fpc | ((fpc & 0x00000300) == 0 ? fpc << 8 : 0))
                 & FE_ALL_EXCEPT & exceptions;
#  endif
  return 0;
}

# elif defined __sh__

/* On glibc, fexcept_t is binary-equivalent to an 'unsigned short'.
   On all other OSes, fexcept_t is binary-equivalent to an 'unsigned int'.
   A simple C cast does the necessary conversion.  */

int
fegetexceptflag (fexcept_t *saved_flags, int exceptions)
{
  /* Just like fetestexcept.  */
  unsigned int fpscr;
  _FPU_GETCW (fpscr);
  *saved_flags = fpscr & FE_ALL_EXCEPT & exceptions;
  return 0;
}

# elif defined __sparc

/* On all OSes except Solaris, fexcept_t is binary-equivalent to an 'unsigned long'.
   On Solaris, fexcept_t is an 'int'.
   A simple C cast does the necessary conversion.  */

int
fegetexceptflag (fexcept_t *saved_flags, int exceptions)
{
  /* Just like fetestexcept.  */
  unsigned long fsr;
  _FPU_GETCW (fsr);
#  if FE_INEXACT == 32 /* glibc compatible FE_* values */
  *saved_flags = fsr & FE_ALL_EXCEPT & exceptions;
#  else /* Solaris compatible FE_* values */
  *saved_flags = (fsr >> 5) & FE_ALL_EXCEPT & exceptions;
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
fegetexceptflag (fexcept_t *saved_flags, int exceptions)
{
  /* Just like fetestexcept.  */
  fp_except_t flags = fpgetsticky ();
  *saved_flags = flags & FE_ALL_EXCEPT & exceptions;
  return 0;
}

# elif defined _AIX && defined _ARCH_PPC /* AIX */

#  include <float.h>
#  include <fpxcp.h>

/* Documentation:
   <https://www.ibm.com/docs/en/aix/7.3?topic=f-fp-clr-flag-fp-set-flag-fp-read-flag-fp-swap-flag-subroutine>  */

int
fegetexceptflag (fexcept_t *saved_flags, int exceptions)
{
  /* Just like fetestexcept.  */
  fpflag_t flags = fp_read_flag ();
  *saved_flags = fpflag_to_exceptions (flags) & FE_ALL_EXCEPT & exceptions;
  return 0;
}

# else

#  define NEED_FALLBACK 1

# endif

#endif

#if NEED_FALLBACK

/* A dummy fallback.  */

int
fegetexceptflag (fexcept_t *saved_flags, int exceptions)
{
  /* Just like fetestexcept.  */
  *saved_flags = 0;
  return 0;
}

#endif
