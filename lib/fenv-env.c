/* Functions for controlling the floating-point environment as a whole.
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

/* Based on glibc/sysdeps/<cpu>/{fegetenv.c,fesetenv.c}
   together with glibc/sysdeps/<cpu>/{fpu_control.h,fenv_private.h,fenv_libc.h}.  */

#include <config.h>

/* Specification.  */
#include <fenv.h>

#include "fenv-private.h"
#include "verify.h"

/* The big problem of the fegetenv/fesetenv API is that it is not well-defined
   which parts of the floating-point environment need to be restored by fesetenv
   and which don't.  For example:
     - On x86_64 and i386, some platforms restore all the x86_387_fenv_t,
       whereas others restore only the control word and the status words.
     - On m68k, musl libc restores the fpiar register value, whereas glibc
       doesn't.
   Some platforms don't even do this consistently.  For example, musl libc on
   sh4 restores the precision mode bit (fpscr bit 19) during an fesetenv of a
   previously saved environment, but not during an fesetenv (FE_DFL_ENV)
   invocation.  */

#if defined _AIX && defined _ARCH_PPC /* AIX */

/* On AIX, fenv_t is a struct { unsigned short rmode; unsigned int fpstat, trapstate; }.  */

/* On AIX, the register fpscr is augmented with a 32-bit word named fpscrx
   in thread-local storage.  Therefore AIX does complex things behind the scene,
   and its best to use the system-provided fegetenv() and fesetenv() functions
   and just add the missing behaviour, namely saving and restoring the exception
   trap bits.  */

# include <float.h>
# include <fpxcp.h>

# include <fptrap.h>

int
fegetenv (fenv_t *envp)
#undef fegetenv
{
  /* Invoke the original fegetenv(), and additionally save the exception trap
     bits (fpscr bits 7..3).  */
  union { unsigned long long u; double f; } memenv;
  _FPU_GETCW_AS_DOUBLE (memenv.f);
  fegetenv (envp);
  /* AIX's original fegetenv() sets envp->trapstate = 0.  */
  envp->trapstate = FE_ALL_EXCEPT & ((unsigned int) memenv.u << 22);
  return 0;
}

int
fesetenv (fenv_t const *envp)
#undef fesetenv
{
  /* Invoke the original fesetenv(), and additionally restore the exception trap
     bits (fpscr bits 7..3).  */
  if (fesetenv (envp) != 0)
    return -1;
  union { unsigned long long u; double f; } orig_memenv, memenv;
  _FPU_GETCW_AS_DOUBLE (orig_memenv.f);
  unsigned int trapbits;
  if (envp == FE_DFL_ENV)
    trapbits = 0;
  else
    trapbits = (envp->trapstate & FE_ALL_EXCEPT) >> 22;
  memenv.u = (orig_memenv.u & ~0x000000f8) | trapbits;
  if (!(memenv.u == orig_memenv.u))
    {
      if ((orig_memenv.u & 0x000000f8) == 0 && (memenv.u & 0x000000f8) != 0)
        {
          /* Put the thread into precise trapping mode.  */
          /* Documentation: <https://www.ibm.com/docs/en/aix/7.3?topic=f-fp-trap-subroutine>  */
          fp_trap (FP_TRAP_SYNC);
        }
      _FPU_SETCW_AS_DOUBLE (memenv.f);
      if ((orig_memenv.u & 0x000000f8) != 0 && (memenv.u & 0x000000f8) == 0)
        {
          /* Put the thread into no-trapping mode.  */
          /* Documentation: <https://www.ibm.com/docs/en/aix/7.3?topic=f-fp-trap-subroutine>  */
          fp_trap (FP_TRAP_OFF);
        }
    }
  return 0;
}

#elif defined __GNUC__ || defined __clang__ || defined _MSC_VER

# if (defined __x86_64__ || defined _M_X64) || (defined __i386 || defined _M_IX86)

/* On all OSes except MSVC, mingw ≥ 13, macOS, Solaris, fenv_t is
   binary-equivalent to
     - either a x86_387_fenv_t (7 'unsigned int' words)
       where mxcsr is stored:
         - for glibc/i386: in __eip = more[1].
         - for musl libc/i386: the lower 6 bits ORed into __status_word.
         - for FreeBSD/i386, Haiku/i386, mingw/i386:
           the upper 16 bits in __reserved1, the lower 16 bits in __reserved2.
     - or a struct { x86_387_fenv_t 387; unsigned int mxcsr; }
   On MSVC, it's a
     struct { unsigned int _Fe_ctl, _Fe_stat; }, where
     _Fe_ctl bits 9..8 are the rounding direction,
     _Fe_ctl bits 4..0 are the exception trap bits (inverted),
     _Fe_stat bits 4..0 are the exception flags.
   On mingw ≥ 13, it's a
     struct { unsigned int _Fe_ctl, _Fe_stat; }, where
     _Fe_ctl bits 9..8 are the rounding direction,
     _Fe_ctl bits 23..22 are the rounding direction
                             from the 387 compatible floating-point unit,
     _Fe_ctl bits 31..30 are the rounding direction
                             from the SSE compatible floating-point unit,
     _Fe_ctl bits 4..0 are the exception trap bits (inverted),
     _Fe_ctl bits 20..16 are the exception trap bits (inverted)
                             from the 387 compatible floating-point unit,
     _Fe_ctl bits 28..24 are the exception trap bits (inverted)
                             from the SSE compatible floating-point unit,
     _Fe_stat bits 4..0 are the exception flags,
     _Fe_stat bits 20..16 are the exception flags
                              from the 387 compatible floating-point unit,
     _Fe_stat bits 28..24 are the exception flags
                              from the SSE compatible floating-point unit.
   On macOS, it's a
     struct { unsigned short __control, __status; unsigned int __mxcsr; ... }.
   On Solaris, it's a
     struct { __fex_handler_t __handlers; unsigned long __fsr; }.  */

/* Like FE_ALL_EXCEPT, except that it also includes FE_DENORMAL.  */
#  define _FE_ALL_EXCEPT (0x3F * FE_INVALID)

int
fegetenv (fenv_t *envp)
{
#  if defined _MSC_VER

  unsigned int mxcsr;
  _FPU_GETSSECW (mxcsr);
  envp->_Fe_ctl = ((mxcsr & 0x6000) >> 5) /* rounding direction */
                  /* exception trap bits (inverted):  */
                  | (mxcsr & (1 << 7) ? FE_INVALID : 0)
                  | (mxcsr & (1 << 9) ? FE_DIVBYZERO : 0)
                  | (mxcsr & (1 << 10) ? FE_OVERFLOW : 0)
                  | (mxcsr & (1 << 11) ? FE_UNDERFLOW : 0)
                  | (mxcsr & (1 << 12) ? FE_INEXACT : 0);
  envp->_Fe_stat = /* exception flags:  */
                     (mxcsr & (1 << 0) ? FE_INVALID : 0)
                   | (mxcsr & (1 << 2) ? FE_DIVBYZERO : 0)
                   | (mxcsr & (1 << 3) ? FE_OVERFLOW : 0)
                   | (mxcsr & (1 << 4) ? FE_UNDERFLOW : 0)
                   | (mxcsr & (1 << 5) ? FE_INEXACT : 0);

#  elif (defined __MINGW32__ && FE_INVALID != 0x01) /* mingw ≥ 13 */

  x86_387_fenv_t env387;

  __asm__ __volatile__ ("fnstenv %0" : "=m" (*&env387));
  /* Note: fnstenv masks all floating-point exceptions until the fldcw
     below.  */
  __asm__ __volatile__ ("fldcw %0" : : "m" (env387.__control_word));

  /* rounding direction */
  unsigned int round_387 = (env387.__control_word & 0x0C00) >> 2;
  /* exception trap bits (inverted) */
  unsigned int trapbits_387 = x86hardware_to_exceptions (env387.__control_word);
  /* exception flags */
  unsigned int excflags_387 = x86hardware_to_exceptions (env387.__status_word);

  unsigned int mxcsr;
  _FPU_GETSSECW (mxcsr);

  /* rounding direction */
  unsigned int round_sse = (mxcsr & 0x6000) >> 5;
  /* exception trap bits (inverted) */
  unsigned int trapbits_sse = x86hardware_to_exceptions (mxcsr >> 7);
  /* exception flags */
  unsigned int excflags_sse = x86hardware_to_exceptions (mxcsr);

  envp->_Fe_ctl =
      (round_sse << 22) | (round_387 << 14) | round_sse | round_387
    | (trapbits_sse << 24) | (trapbits_387 << 16) | trapbits_sse | trapbits_387;
  envp->_Fe_stat =
    (excflags_sse << 24) | (excflags_387 << 16) | excflags_sse | excflags_387;

#  elif defined __APPLE__ && defined __MACH__ /* macOS */

  _FPU_GETCW (envp->__control);
  _FPU_GETSTAT (envp->__status);
  _FPU_GETSSECW (envp->__mxcsr);

#  elif defined __sun /* Solaris */

  fex_getexcepthandler (&envp->__handlers, FEX_ALL);

  unsigned short fctrl;
  unsigned short fstat;
  unsigned int mxcsr;
  _FPU_GETCW (fctrl);
  _FPU_GETSTAT (fstat);
  _FPU_GETSSECW (mxcsr);
  envp->__fsr =
    (((unsigned int) (fctrl ^ 0x3F) & ~0xE0C0U) << 16)
    | ((unsigned int) fstat | (mxcsr & 0x3F));

#  else

  verify (sizeof (fenv_t) >= sizeof (x86_387_fenv_t));

  __asm__ __volatile__ ("fnstenv %0" : "=m" (* (x86_387_fenv_t *) envp));
  /* Note: fnstenv masks all floating-point exceptions until the fldcw
     below.  */
  __asm__ __volatile__ ("fldcw %0" : : "m" (((x86_387_fenv_t *) envp)->__control_word));

  unsigned int mxcsr;
  _FPU_GETSSECW (mxcsr);
  if (sizeof (fenv_t) > sizeof (x86_387_fenv_t))
    {
      /* Store mxcsr in the struct field after the x86_387_fenv_t.  */
      * (unsigned int *) ((x86_387_fenv_t *) envp + 1) = mxcsr;
    }
  else
    {
#   if defined __GLIBC__
      ((x86_387_fenv_t *) envp)->more[1] = mxcsr;
#   elif MUSL_LIBC
      ((x86_387_fenv_t *) envp)->__status_word |= mxcsr & 0x3F;
#   else
      ((x86_387_fenv_t *) envp)->__reserved1 = mxcsr >> 16;
      ((x86_387_fenv_t *) envp)->__reserved2 = mxcsr & 0xFFFF;
#   endif
    }

#  endif
  return 0;
}

int
fesetenv (fenv_t const *envp)
{
#  if defined _MSC_VER

  unsigned int mxcsr;
  _FPU_GETSSECW (mxcsr);
  mxcsr &= ~(0x6000 | (0x3F << 7) | 0x3F);
  /* On MSVC, FE_DFL_ENV is the address of a global variable.
     Let's ignore this variable.  */
  if (envp == FE_DFL_ENV)
    /* Default: rounding direction = FE_TONEAREST, exception trap bits = 0,
       exception flags = 0.  */
    mxcsr = mxcsr | (0x3F << 7);
  else
    mxcsr = mxcsr
            | ((envp->_Fe_ctl & 0x300) << 5) /* rounding direction */
            /* exception trap bits (inverted):  */
            | (envp->_Fe_ctl & FE_INVALID   ? 1 << 7 : 0)
            | (envp->_Fe_ctl & FE_DIVBYZERO ? 1 << 9 : 0)
            | (envp->_Fe_ctl & FE_OVERFLOW  ? 1 << 10 : 0)
            | (envp->_Fe_ctl & FE_UNDERFLOW ? 1 << 11 : 0)
            | (envp->_Fe_ctl & FE_INEXACT   ? 1 << 12 : 0)
            /* exception flags:  */
            | (envp->_Fe_stat & FE_INVALID   ? 1 << 0 : 0)
            | (envp->_Fe_stat & FE_DIVBYZERO ? 1 << 2 : 0)
            | (envp->_Fe_stat & FE_OVERFLOW  ? 1 << 3 : 0)
            | (envp->_Fe_stat & FE_UNDERFLOW ? 1 << 4 : 0)
            | (envp->_Fe_stat & FE_INEXACT   ? 1 << 5 : 0);
  _FPU_SETSSECW (mxcsr);

#  elif (defined __MINGW32__ && FE_INVALID != 0x01) /* mingw ≥ 13 */

  unsigned short env_fctrl;
  unsigned short env_fstat;
  unsigned int env_mxcsr;
  /* On mingw, FE_DFL_ENV is NULL.  */
  if (envp == FE_DFL_ENV)
    {
      env_fctrl = 0x3F;
      env_fstat = 0;
      env_mxcsr = 0x3F << 7;
    }
  else
    {
      /* rounding direction */
      unsigned int round_387 = (envp->_Fe_ctl >> 14) & 0x0300;
      unsigned int round_sse = (envp->_Fe_ctl >> 22) & 0x0300;
      /* exception trap bits (inverted) */
      unsigned int trapbits_387 = (envp->_Fe_ctl >> 16) & 0x1F;
      unsigned int trapbits_sse = (envp->_Fe_ctl >> 24) & 0x1F;
      /* exception flags */
      unsigned int excflags_387 = (envp->_Fe_stat >> 16) & 0x1F;
      unsigned int excflags_sse = (envp->_Fe_stat >> 24) & 0x1F;

      env_fctrl = (round_387 << 2) | exceptions_to_x86hardware (trapbits_387);
      env_fstat = exceptions_to_x86hardware (excflags_387);
      env_mxcsr = (round_sse << 5)
                  | (exceptions_to_x86hardware (trapbits_sse) << 7)
                  | exceptions_to_x86hardware (excflags_sse);
    }

  /* In the SSE unit.  */
  unsigned int mxcsr = env_mxcsr;
  _FPU_SETSSECW (mxcsr);

  /* In the 387 unit.  */
  x86_387_fenv_t env387;
  __asm__ __volatile__ ("fnstenv %0" : "=m" (*&env387));
  /* Note: fnstenv masks all floating-point exceptions until the fldenv
     below.  */
  env387.__control_word = env_fctrl;
  env387.__status_word = env_fstat;
  __asm__ __volatile__ ("fldenv %0" : : "m" (*&env387));

#  elif defined __APPLE__ && defined __MACH__ /* macOS */

  unsigned short fctrl;
  unsigned short fstat;
  unsigned int mxcsr;
  /* On macOS, FE_DFL_ENV is the address of a global variable;
     no special code is needed in this case.  */
  if (FE_DFL_ENV == (const fenv_t *) (-1) && envp == FE_DFL_ENV)
    {
      fctrl = 0x3F;
      fstat = 0;
      mxcsr = 0x3F << 7;
    }
  else
    {
      fctrl = envp->__control;
      fstat = envp->__status;
      mxcsr = envp->__mxcsr;
    }
  /* In the SSE unit.  */
  _FPU_SETSSECW (mxcsr);

  /* In the 387 unit.  */
  x86_387_fenv_t env387;
  __asm__ __volatile__ ("fnstenv %0" : "=m" (*&env387));
  /* Note: fnstenv masks all floating-point exceptions until the fldenv
     below.  */
  env387.__control_word = fctrl;
  env387.__status_word = fstat;
  __asm__ __volatile__ ("fldenv %0" : : "m" (*&env387));

#  elif defined __sun /* Solaris */

  unsigned short env_fctrl;
  unsigned int env_mxcsr;
  /* On Solaris, FE_DFL_ENV is the address of a global variable;
     no special code is needed in this case.  */
  if (FE_DFL_ENV == (const fenv_t *) (-1) && envp == FE_DFL_ENV)
    {
      env_fctrl = 0x3F;
      env_mxcsr = 0x3F << 7;
    }
  else
    {
      fex_setexcepthandler (&envp->__handlers, FEX_ALL);
      env_fctrl = (envp->__fsr >> 16) ^ 0x3F;
      env_mxcsr = envp->__fsr & 0x3F;
    }

  /* Store the exception flags in mxcsr, not in env387.__status_word.  */

  /* In the SSE unit.  */
  unsigned int mxcsr;
  _FPU_GETSSECW (mxcsr);
  mxcsr = (mxcsr & ~0x3F) | env_mxcsr;
  _FPU_SETSSECW (mxcsr);

  /* In the 387 unit.  */
  x86_387_fenv_t env387;
  __asm__ __volatile__ ("fnstenv %0" : "=m" (*&env387));
  /* Note: fnstenv masks all floating-point exceptions until the fldenv
     below.  */
  env387.__control_word = env_fctrl;
  env387.__status_word &= ~0x3F;
  __asm__ __volatile__ ("fldenv %0" : : "m" (*&env387));

#  else

  verify (sizeof (fenv_t) >= sizeof (x86_387_fenv_t));

  unsigned short env_fctrl;
  unsigned short env_fstat;
  unsigned int env_mxcsr;
  /* On *BSD, Solaris, Cygwin, Android, Haiku, Minix, FE_DFL_ENV is the address
     of a global variable; no special code is needed in this case.
     But on mingw, FE_DFL_ENV is NULL.  */
  if (envp == FE_DFL_ENV)
    {
      env_fctrl = 0x3F;
      env_fstat = 0;
      env_mxcsr = 0x3F << 7;
    }
  else
    {
      env_fctrl = ((x86_387_fenv_t const *) envp)->__control_word;
      env_fstat = ((x86_387_fenv_t const *) envp)->__status_word;
      if (sizeof (fenv_t) > sizeof (x86_387_fenv_t))
        env_mxcsr = * (unsigned int const *) ((x86_387_fenv_t const *) envp + 1);
      else
        {
#   if defined __GLIBC__
          env_mxcsr = ((x86_387_fenv_t const *) envp)->more[1];
#   elif MUSL_LIBC
          env_mxcsr = env_fstat & 0x3F;
#   else
          env_mxcsr = (((x86_387_fenv_t const *) envp)->__reserved1 << 16)
                      | ((x86_387_fenv_t const *) envp)->__reserved2;
#   endif
        }
    }

  /* In the SSE unit.  */
  unsigned int mxcsr = env_mxcsr;
  _FPU_SETSSECW (mxcsr);

  /* In the 387 unit.  */
  x86_387_fenv_t env387;
  __asm__ __volatile__ ("fnstenv %0" : "=m" (*&env387));
  /* Note: fnstenv masks all floating-point exceptions until the fldenv
     below.  */
  env387.__control_word = env_fctrl;
  env387.__status_word = env_fstat;
  __asm__ __volatile__ ("fldenv %0" : : "m" (*&env387));

#  endif

  return 0;
}

# elif defined __aarch64__ /* arm64 */

/* On all OSes except FreeBSD and macOS, fenv_t is binary-equivalent to a
   struct { unsigned int fpcr, fpsr; }.
   On FreeBSD, it's binary-equivalent to a
   struct { unsigned int fpsr, fpcr; }, i.e. the fields are swapped.
   On macOS, it's binary-equivalent to a
   struct { unsigned long fpsr, fpcr; }.  */

int
fegetenv (fenv_t *envp)
{
  unsigned long fpcr;
  unsigned long fpsr;
  _FPU_GETCW (fpcr);
  _FPU_GETFPSR (fpsr);
#  if defined __APPLE__ && defined __MACH__
  ((unsigned long *) envp)[0] = fpsr;
  ((unsigned long *) envp)[1] = fpcr;
#  elif defined __FreeBSD__
  ((unsigned int *) envp)[0] = fpsr;
  ((unsigned int *) envp)[1] = fpcr;
#  else
  ((unsigned int *) envp)[0] = fpcr;
  ((unsigned int *) envp)[1] = fpsr;
#  endif
  return 0;
}

int
fesetenv (fenv_t const *envp)
{
  unsigned long orig_fpcr, fpcr;
  unsigned long orig_fpsr, fpsr;
  _FPU_GETCW (orig_fpcr);
  _FPU_GETFPSR (orig_fpsr);
  /* On *BSD and Android, FE_DFL_ENV is the address of a global variable;
     no special code is needed in this case.  */
  if (FE_DFL_ENV == (const fenv_t *) (-1) && envp == FE_DFL_ENV)
    {
      /* Default: rounding direction = FE_TONEAREST, exceptions trap bits = 0. */
      fpcr = (orig_fpcr & 0xFE0FE0F8U) | 0x00000000U;
      /* Default: exception flags = 0.  */
      fpsr = (orig_fpsr & 0x0FFFFFE0U) | 0x00000000U;
    }
  else
    {
#  if defined __APPLE__ && defined __MACH__
      fpsr = ((unsigned long const *) envp)[0];
      fpcr = ((unsigned long const *) envp)[1];
#  elif defined __FreeBSD__
      fpsr = ((unsigned int const *) envp)[0];
      fpcr = ((unsigned int const *) envp)[1];
#  else
      fpcr = ((unsigned int const *) envp)[0];
      fpsr = ((unsigned int const *) envp)[1];
#  endif
    }
  if (fpcr != orig_fpcr)
    _FPU_SETCW (fpcr);
  if (fpsr != orig_fpsr)
    _FPU_SETFPSR (fpsr);
  return 0;
}

# elif defined __arm__

/* On all OSes except OpenBSD, fenv_t is binary-equivalent to an 'unsigned int'.
   On OpenBSD, it's a struct { unsigned int __sticky, __mask, __round; }.  */

int
fegetenv (fenv_t *envp)
{
#  ifdef __SOFTFP__
  return -1;
#  else
  unsigned int fpscr;
  _FPU_GETCW (fpscr);
#   if defined __OpenBSD__
  envp->__sticky = fpscr & FE_ALL_EXCEPT;
  envp->__mask = (fpscr >> 8) & FE_ALL_EXCEPT;
  envp->__round = (fpscr >> 22) & 3;
#   else
  * (unsigned int *) envp = fpscr;
#   endif
  return 0;
#  endif
}

int
fesetenv (fenv_t const *envp)
{
#  ifdef __SOFTFP__
  return -1;
#  else
  unsigned int fpscr, orig_fpscr;
  _FPU_GETCW (orig_fpscr);
  /* On *BSD and Android, FE_DFL_ENV is the address of a global variable;
     no special code is needed in this case.  */
  if (FE_DFL_ENV == (const fenv_t *) (-1) && envp == FE_DFL_ENV)
    /* Default: rounding direction = FE_TONEAREST, exception trap bits = 0,
       exception flags = 0.  */
    fpscr = (orig_fpscr & 0x00086060) | 0x00000000;
  else
    /* Here, glibc ignores orig_fpscr entirely...  */
    fpscr = (orig_fpscr & ~((3U << 22) | (FE_ALL_EXCEPT << 8) | FE_ALL_EXCEPT))
#   if defined __OpenBSD__
      | ((envp->__round & 3U) << 22)
      | ((envp->__mask & FE_ALL_EXCEPT) << 8)
      | (envp->__sticky & FE_ALL_EXCEPT)
#   else
      | (* (unsigned int const *) envp & ((3U << 22) | (FE_ALL_EXCEPT << 8) | FE_ALL_EXCEPT))
#   endif
      ;
  if (((fpscr ^ orig_fpscr) & ~0xF0000000) != 0)
    _FPU_SETCW (fpscr);
  return 0;
#  endif
}

# elif defined __alpha

/* On all OSes except OpenBSD, fenv_t is binary-equivalent to an 'unsigned long'.
   On OpenBSD, it's a struct { unsigned int __sticky, __mask, __round; }.  */

/* Like FE_ALL_EXCEPT, except that it also includes FE_DENORMAL.  */
#  define _FE_ALL_EXCEPT (0x3FUL * FE_INVALID)

int
fegetenv (fenv_t *envp)
{
  unsigned long swcr = __ieee_get_fp_control ();
  unsigned long fpcr;
  _FPU_GETCW (fpcr);
#   if defined __OpenBSD__
  envp->__sticky = (swcr >> 17) & _FE_ALL_EXCEPT;
  envp->__mask = (swcr >> 1) & _FE_ALL_EXCEPT;
  envp->__round = (fpcr >> 58) & 0x3UL;
#   else
  * (unsigned long *) envp =
    (fpcr & 0x0C00000000000000UL) /* rounding direction */
    | (swcr & 0x007E307EUL); /* exception flags,
                                mapping of denormal inputs and outputs,
                                exception trap bits */
#   endif
  return 0;
}

int
fesetenv (fenv_t const *envp)
{
  unsigned long env;
  /* On *BSD, FE_DFL_ENV is the address of a global variable;
     on glibc, it is a magic address.  */
  if (envp == FE_DFL_ENV)
    /* Default: rounding direction = FE_TONEAREST, exception flags = 0,
       mapping = 0, exception trap bits = 0.  */
    env = 0x0800000000000000UL;
  else
    {
#   if defined __OpenBSD__
      env = ((unsigned long) (envp->__round & 0x3UL) << 58)
            | ((envp->__sticky & _FE_ALL_EXCEPT) << 17)
            | ((envp->__mask & _FE_ALL_EXCEPT) << 1);
#   else
      env = * (unsigned long const *) envp;
#   endif
    }

  /* Set the rounding direction.  */
  unsigned long fpcr, orig_fpcr;
  _FPU_GETCW (orig_fpcr);
  fpcr = (orig_fpcr & ~0x0C00000000000000UL) | (env & 0x0C00000000000000UL);
  if (fpcr != orig_fpcr)
    _FPU_SETCW (fpcr);
  /* Set the exception flags, the mapping of denormal inputs and outputs, and
     the exception trap bits.  */
  __ieee_set_fp_control (env & 0x007E307EUL);

  return 0;
}

# elif defined __hppa

/* On all OSes except glibc, fenv_t is binary-equivalent to an 'unsigned int'.
   On glibc, it's a struct { unsigned int __status_word, __exception[7]; },
   of which only the __status_word field is used.  */

int
fegetenv (fenv_t *envp)
{
  unsigned int fpstatus;
  _FPU_GETCW (fpstatus);
  * (unsigned int *) envp = fpstatus;
  return 0;
}

int
fesetenv (fenv_t const *envp)
{
  unsigned int fpstatus, orig_fpstatus;
  _FPU_GETCW (orig_fpstatus);
  /* On *BSD, FE_DFL_ENV is the address of a global variable;
     no special code is needed in this case.  */
  if (FE_DFL_ENV == (const fenv_t *) (-1) && envp == FE_DFL_ENV)
    /* Default: exception flags = 0, rounding direction = FE_TONEAREST,
       exception trap bits = 0.  */
    fpstatus = 0x00000000U;
  else
    {
      unsigned int env = * (unsigned int const *) envp;
      fpstatus = (orig_fpstatus & ~0xF800061FU) | (env & 0xF800061FU);
    }
  if (fpstatus != orig_fpstatus)
    _FPU_SETCW (fpstatus);
  return 0;
}

# elif defined __ia64__

/* On all OSes, fenv_t is binary-equivalent to an 'unsigned long'.  */

/* Like FE_ALL_EXCEPT, except that it also includes FE_DENORMAL.  */
#  define _FE_ALL_EXCEPT (0x3FUL * FE_INVALID)

int
fegetenv (fenv_t *envp)
{
  unsigned long fpsr;
  _FPU_GETCW (fpsr);
  * (unsigned long *) envp = fpsr;
  return 0;
}

int
fesetenv (fenv_t const *envp)
{
  unsigned long env;
  /* On NetBSD, FE_DFL_ENV is the address of a global variable;
     on glibc, it is a magic address.  */
  if (envp == FE_DFL_ENV)
    /* Default:
       sf0 = 0b0000000001100, sf1 = 0b0000001001110, sf2 = sf3 = 0000001001100
       i.e. precision control = 80-bits "extended",
            rounding direction = FE_TONEAREST,
            exception flags = 0,
       exceptions trap bits = all 1, i.e. all masked.  */
    env = 0x0009804C0270033FUL;
  else
    env = * (unsigned long const *) envp;
  unsigned long fpsr = env;
  _FPU_SETCW (fpsr);
  return 0;
}

# elif defined __m68k__

/* On all OSes, fenv_t is binary-equivalent to a struct
   { unsigned int control_register, status_register, instruction_address; }.  */

#  define _FPU_GETFPIAR(cw) __asm__ __volatile__ ("fmove%.l %/fpiar, %0" : "=dm" (cw))
#  define _FPU_SETFPIAR(cw) __asm__ __volatile__ ("fmove%.l %0, %/fpiar" : : "dm" (cw))

int
fegetenv (fenv_t *envp)
{
  unsigned int fpcr;
  unsigned int fpsr;
  unsigned int fpiar;
  _FPU_GETCW (fpcr);
  _FPU_GETFPSR (fpsr);
  _FPU_GETFPIAR (fpiar);
  ((unsigned int *) envp)[0] = fpcr;
  ((unsigned int *) envp)[1] = fpsr;
  ((unsigned int *) envp)[2] = fpiar;
  return 0;
}

int
fesetenv (fenv_t const *envp)
{
  unsigned int env_fpcr;
  unsigned int env_fpsr;
  if (envp == FE_DFL_ENV)
    {
      /* Default: exceptions trap bits = 0, rounding direction = FE_TONEAREST.  */
      env_fpcr = 0;
      /* Default: exception flags = 0.  */
      env_fpsr = 0;
    }
  else
    {
      /* No need to restore the instruction address here.  */
      env_fpcr = ((unsigned int const *) envp)[0];
      env_fpsr = ((unsigned int const *) envp)[1];
    }
  unsigned int fpcr, orig_fpcr;
  unsigned int fpsr, orig_fpsr;
  unsigned int fpiar, orig_fpiar;
  _FPU_GETCW (orig_fpcr);
  _FPU_GETFPSR (orig_fpsr);
  _FPU_GETFPIAR (orig_fpiar);
  /* glibc uses 0x3E30U here.  I think 0xFF30U is a better choice.  */
  fpcr = (orig_fpcr & ~0xFF30U) | (env_fpcr & 0xFF30U);
  fpsr = (orig_fpsr & ~0x00F8U) | (env_fpsr & 0x00F8U);
  fpiar = orig_fpiar;
  _FPU_SETCW (fpcr);
  _FPU_SETFPSR (fpsr);
  _FPU_SETFPIAR (fpiar);
  return 0;
}

# elif defined __mips__

/* On all OSes, fenv_t is binary-equivalent to an 'unsigned int'.  */

int
fegetenv (fenv_t *envp)
{
  unsigned int fcsr;
  _FPU_GETCW (fcsr);
  * (unsigned int *) envp = fcsr;
  return 0;
}

int
fesetenv (fenv_t const *envp)
{
  unsigned int env;
  /* On *BSD, FE_DFL_ENV is the address of a global variable;
     no special code is needed in this case.  */
  if (FE_DFL_ENV == (const fenv_t *) (-1) && envp == FE_DFL_ENV)
    /* Default: exceptions trap bits = 0, exception flags = 0,
       rounding direction = FE_TONEAREST.  */
    env = 0x00000000U;
  else
    env = * (unsigned int const *) envp;
  unsigned int fcsr = env;
  _FPU_SETCW (fcsr);
  return 0;
}

# elif defined __loongarch__

/* On all OSes, fenv_t is binary-equivalent to an 'unsigned int'.  */

int
fegetenv (fenv_t *envp)
{
  unsigned int fcsr;
  _FPU_GETCW (fcsr);
  * (unsigned int *) envp = fcsr;
  return 0;
}

int
fesetenv (fenv_t const *envp)
{
  unsigned int env;
  if (envp == FE_DFL_ENV)
    /* Default: exception flags = 0, rounding direction = FE_TONEAREST,
       exceptions trap bits = 0.  */
    env = 0x00000000U;
  else
    env = * (unsigned int const *) envp;
  unsigned int fcsr = env;
  _FPU_SETCW (fcsr);
  return 0;
}

# elif defined _ARCH_PPC

/* On all OSes except *BSD and AIX, fenv_t is a 'double'.
   On *BSD, it's an 'unsigned int'.
   On AIX, it's a struct { unsigned short rmode; unsigned int fpstat, trapstate; }.
   But AIX has already been dealt with above.  */

#  if defined __linux__
#   include <sys/prctl.h>
#  endif

int
fegetenv (fenv_t *envp)
{
#  if defined __FreeBSD__ || defined __NetBSD__ || defined __OpenBSD__
  union { unsigned long long u; double f; } memenv;
  _FPU_GETCW_AS_DOUBLE (memenv.f);
  * (unsigned int *) envp = (unsigned int) memenv.u;
#  else
  _FPU_GETCW_AS_DOUBLE (*envp);
#  endif
  return 0;
}

int
fesetenv (fenv_t const *envp)
{
  union { unsigned long long u; double f; } orig_memenv, memenv;
  _FPU_GETCW_AS_DOUBLE (orig_memenv.f);
  /* On glibc and *BSD, FE_DFL_ENV is the address of a global variable;
     no special code is needed in this case.  */
  if (FE_DFL_ENV == (const fenv_t *) (-1) && envp == FE_DFL_ENV)
    memenv.u = 0xFFF8000000000000ULL;
  else
#  if defined __FreeBSD__ || defined __NetBSD__ || defined __OpenBSD__
    memenv.u = * (unsigned int const *) envp;
#  else
    memenv.f = *envp;
#  endif
  if (!(memenv.u == orig_memenv.u))
    {
      if ((orig_memenv.u & 0x000000f8) == 0 && (memenv.u & 0x000000f8) != 0)
        {
          /* Put the thread into precise trapping mode.  */
#  if defined __linux__ || defined __NetBSD__
          prctl (PR_SET_FPEXC, PR_FP_EXC_PRECISE);
#  endif
        }
      _FPU_SETCW_AS_DOUBLE (memenv.f);
      if ((orig_memenv.u & 0x000000f8) != 0 && (memenv.u & 0x000000f8) == 0)
        {
          /* Put the thread into no-trapping mode.  */
#  if defined __linux__ || defined __NetBSD__
          prctl (PR_SET_FPEXC, PR_FP_EXC_DISABLED);
#  endif
        }
    }
  return 0;
}

# elif defined __riscv

/* On all OSes except FreeBSD, fenv_t is binary-equivalent to an 'unsigned int'.
   On FreeBSD, it's binary-equivalent to an 'unsigned long'.  */

#  define _FPU_GETCW(cw) __asm__ __volatile__ ("frcsr %0" : "=r" (cw))
#  define _FPU_SETCW(cw) __asm__ __volatile__ ("fscsr %z0" : : "rJ" (cw))

int
fegetenv (fenv_t *envp)
{
  unsigned int fcsr;
  _FPU_GETCW (fcsr);
#  if defined __FreeBSD__
  * (unsigned long *) envp = fcsr;
#  else
  * (unsigned int *) envp = fcsr;
#  endif
  return 0;
}

int
fesetenv (fenv_t const *envp)
{
  unsigned int env;
  /* On *BSD and Android, FE_DFL_ENV is the address of a global variable;
     no special code is needed in this case.  */
  if (FE_DFL_ENV == (const fenv_t *) (-1) && envp == FE_DFL_ENV)
    /* Default: rounding direction = FE_TONEAREST, exception flags = 0.  */
    env = 0x00000000U;
  else
#  if defined __FreeBSD__
    env = * (unsigned long const *) envp;
#  else
    env = * (unsigned int const *) envp;
#  endif
  unsigned int fcsr = env;
  _FPU_SETCW (fcsr);
  return 0;
}

# elif defined __s390__ || defined __s390x__

/* On all OSes, fenv_t is binary-equivalent to a struct whose first (and only
   relevant) field is an 'unsigned int'.  */

int
fegetenv (fenv_t *envp)
{
  unsigned int fpc;
  _FPU_GETCW (fpc);
  * (unsigned int *) envp = fpc;
  return 0;
}

int
fesetenv (fenv_t const *envp)
{
  unsigned int env;
  if (envp == FE_DFL_ENV)
    /* Default: exceptions trap bits = 0, exception flags = 0,
       rounding direction = FE_TONEAREST.  */
    env = 0x00000000U;
  else
    env = * (unsigned int const *) envp;
  unsigned int fpc = env;
  _FPU_SETCW (fpc);
  return 0;
}

# elif defined __sh__

/* On all OSes, fenv_t is binary-equivalent to an 'unsigned int'.  */

int
fegetenv (fenv_t *envp)
{
  unsigned int fpscr;
  _FPU_GETCW (fpscr);
  * (unsigned int *) envp = fpscr;
  return 0;
}

int
fesetenv (fenv_t const *envp)
{
  unsigned int env;
  /* On OpenBSD, FE_DFL_ENV is the address of a global variable;
     no special code is needed in this case.  */
  if (FE_DFL_ENV == (const fenv_t *) (-1) && envp == FE_DFL_ENV)
    /* Default: PR = 1, exceptions trap bits = 0, exception flags = 0,
       rounding direction = FE_TONEAREST.  */
    env = 0x00080000U;
  else
    env = * (unsigned int const *) envp;
  unsigned int fpscr = env;
  _FPU_SETCW (fpscr);
  return 0;
}

# elif defined __sparc

/* On all OSes except Solaris, fenv_t is binary-equivalent to an 'unsigned long'.
   On Solaris, it's a struct { __fex_handler_t __handlers; unsigned long __fsr; }.  */

int
fegetenv (fenv_t *envp)
{
  unsigned long fsr;
  _FPU_GETCW (fsr);
#  if defined __sun
  fex_getexcepthandler (&envp->__handlers, FEX_ALL);
  envp->__fsr = fsr;
#  else
  * (unsigned long *) envp = fsr;
#  endif
  return 0;
}

int
fesetenv (fenv_t const *envp)
{
  unsigned long fsr;
  /* On *BSD and Solaris, FE_DFL_ENV is the address of a global variable;
     no special code is needed in this case.  */
  if (FE_DFL_ENV == (const fenv_t *) (-1) && envp == FE_DFL_ENV)
    {
      /* Default: rounding direction = FE_TONEAREST, exceptions trap bits = 0,
         exception flags = 0.  */
      fsr = 0x00000000U;
    }
  else
    {
#  if defined __sun
      fex_setexcepthandler (&envp->__handlers, FEX_ALL);
      fsr = envp->__fsr;
#  else
      fsr = * (unsigned long const *) envp;
#  endif
    }
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

/* Like FE_ALL_EXCEPT, except that it also includes FE_DENORMAL.  */
#  define _FE_ALL_EXCEPT (0x3F * FE_INVALID)

/* Accessors for the 387 unit's special registers.  */

static void
getfctrl (unsigned short *fctrl_p)
{
#  if defined __x86_64__ || defined _M_X64
  asm ("fnstcw (%rdi)");
#  else
  /* The compiler generates a stack frame.  Therefore the first argument is in
     8(%ebp), not in 4(%esp).  */
  asm ("movl 8(%ebp),%eax");
  asm ("fnstcw (%eax)");
#  endif
}

static void
getfstat (unsigned short *fstat_p)
{
#  if defined __x86_64__ || defined _M_X64
  asm ("fnstsw (%rdi)");
#  else
  /* The compiler generates a stack frame.  Therefore the first argument is in
     8(%ebp), not in 4(%esp).  */
  asm ("movl 8(%ebp),%eax");
  asm ("fnstsw (%eax)");
#  endif
}

static void
getenv387 (x86_387_fenv_t *env387_p)
{
#  if defined __x86_64__ || defined _M_X64
  asm ("fnstenv (%rdi)");
#  else
  /* The compiler generates a stack frame.  Therefore the first argument is in
     8(%ebp), not in 4(%esp).  */
  asm ("movl 8(%ebp),%eax");
  asm ("fnstenv (%eax)");
#  endif
}

static void
setenv387 (x86_387_fenv_t const *env387_p)
{
#  if defined __x86_64__ || defined _M_X64
  asm ("fldenv (%rdi)");
#  else
  /* The compiler generates a stack frame.  Therefore the first argument is in
     8(%ebp), not in 4(%esp).  */
  asm ("movl 8(%ebp),%eax");
  asm ("fldenv (%eax)");
#  endif
}

/* Accessors for the mxcsr register.  */

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
fegetenv (fenv_t *envp)
{
  fex_getexcepthandler (&envp->__handlers, FEX_ALL);

  unsigned short fctrl;
  unsigned short fstat;
  unsigned int mxcsr;
  getfctrl (&fctrl);
  getfstat (&fstat);
  getssecw (&mxcsr);
  envp->__fsr =
    (((unsigned int) (fctrl ^ 0x3F) & ~0xE0C0U) << 16)
    | ((unsigned int) fstat | (mxcsr & 0x3F));

  return 0;
}

int
fesetenv (fenv_t const *envp)
{
  unsigned short env_fctrl;
  unsigned int env_mxcsr;
  /* On Solaris, FE_DFL_ENV is the address of a global variable;
     no special code is needed in this case.  */
  if (FE_DFL_ENV == (const fenv_t *) (-1) && envp == FE_DFL_ENV)
    {
      env_fctrl = 0x3F;
      env_mxcsr = 0x3F << 7;
    }
  else
    {
      fex_setexcepthandler (&envp->__handlers, FEX_ALL);
      env_fctrl = (envp->__fsr >> 16) ^ 0x3F;
      env_mxcsr = envp->__fsr & 0x3F;
    }

  /* Store the exception flags in mxcsr, not in env387.__status_word.  */

  /* In the SSE unit.  */
  unsigned int mxcsr;
  getssecw (&mxcsr);
  mxcsr = (mxcsr & ~0x3F) | env_mxcsr;
  setssecw (&mxcsr);

  /* In the 387 unit.  */
  x86_387_fenv_t env387;
  getenv387 (&env387);
  /* Note: fnstenv masks all floating-point exceptions until the setenv387
     below.  */
  env387.__control_word = env_fctrl;
  env387.__status_word &= ~0x3F;
  setenv387 (&env387);

  return 0;
}

#elif defined __sun && defined __sparc && defined __SUNPRO_C
/* Solaris/sparc.  */

/* Accessors for the fsr register.  */

static void
getfsr (unsigned long *fsr_p)
{
#  if defined __sparcv9 || defined __arch64__ /* sparc64 */
  asm ("stx %fsr,[%i0]");
#  else
  asm ("st %fsr,[%i0]");
#  endif
}

static void
setfsr (unsigned long const *fsr_p)
{
#  if defined __sparcv9 || defined __arch64__ /* sparc64 */
  asm ("ldx [%i0],%fsr");
#  else
  asm ("ld [%i0],%fsr");
#  endif
}

int
fegetenv (fenv_t *envp)
{
  unsigned long fsr;
  getfsr (&fsr);
  fex_getexcepthandler (&envp->__handlers, FEX_ALL);
  envp->__fsr = fsr;
  return 0;
}

int
fesetenv (fenv_t const *envp)
{
  unsigned long fsr;
  /* On *BSD and Solaris, FE_DFL_ENV is the address of a global variable;
     no special code is needed in this case.  */
  if (FE_DFL_ENV == (const fenv_t *) (-1) && envp == FE_DFL_ENV)
    {
      /* Default: rounding direction = FE_TONEAREST, exceptions trap bits = 0,
         exception flags = 0.  */
      fsr = 0x00000000U;
    }
  else
    {
      fex_setexcepthandler (&envp->__handlers, FEX_ALL);
      fsr = envp->__fsr;
    }
  setfsr (&fsr);
  return 0;
}

# else

#  define NEED_FALLBACK 1

# endif

#endif

#if NEED_FALLBACK

/* A dummy fallback.  */

int
fegetenv (fenv_t *envp)
{
  return -1;
}

int
fesetenv (fenv_t const *envp)
{
  return -1;
}

#endif
