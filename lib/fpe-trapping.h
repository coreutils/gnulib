/* Trapping floating-point exceptions.
   Copyright (C) 2023 Free Software Foundation, Inc.

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

/* HAVE_FPE_TRAPPING
   Defined to 1 when sigfpe_on_invalid is available.  */

/* sigfpe_on_invalid
   Enables a SIGFPE signal when an FE_INVALID exception occurs.
   A SIGFPE signal by default terminates the program.
   Returns >= 0 when successful, -1 upon failure.  */


#define HAVE_FPE_TRAPPING 1

#include <fenv.h>

#if HAVE_FEENABLEEXCEPT
/* glibc, FreeBSD ≥ 6.0, NetBSD ≥ 7.0, OpenBSD ≥ 5.0, Cygwin ≥ 1.7.8, Android, Haiku.  */

static int
sigfpe_on_invalid ()
{
  /* Clear FE_INVALID exceptions from past operations.  */
  feclearexcept (FE_INVALID);

  /* An FE_INVALID exception shall trigger a SIGFPE signal.
     This call may fail on arm, arm64, riscv64 CPUs.
     Also, possibly a bug in glibc/sysdeps/m68k/fpu/feenablxcpt.c: it sets
     only bit 13, but should better set both bit 14 and bit 13 of the
     control register.  */
  int ret = feenableexcept (FE_INVALID);
  if (ret == -1)
    return -1;

  #if ((__GLIBC__ == 2 && __GLIBC_MINOR__ < 20) || defined __FreeBSD__ || defined __NetBSD__) && defined __aarch64__
  /* Work around a bug with glibc 2.19 and FreeBSD 12.2 on arm64 CPUs:
     feenableexcept returns success even if the CPU does not support the
     request.  */
  fenv_t env;
  fegetenv (&env);
  #if __GLIBC__ >= 2 || defined __NetBSD__
  /* fenv_t is a struct { unsigned int __fpcr, __fpsr; }  */
  if (((FE_INVALID << 8) & ~env.__fpcr) != 0)
    return -1;
  #else /* defined __FreeBSD__ */
  /* fenv_t is an 'uint64_t' that merges fpcr and fpsr.
     But watch out for this incompatible change:
     <https://cgit.freebsd.org/src/commit/?id=34cc08e336987a8ebc316595e3f552a4c09f1fd4>  */
  #if __FreeBSD__ < 14
  if (((FE_INVALID << 8) & ~env) != 0)
  #else
  if (((FE_INVALID << 8) & ~(env >> 32)) != 0)
  #endif
    return -1;
  #endif
  #endif

  #if (defined __FreeBSD__ || defined __NetBSD__) && defined __arm__
  /* Work around a bug with FreeBSD 12.2 on arm CPUs:
     feenableexcept returns success even if the CPU does not support the
     request.  */
  /* Test whether fpscr was actually changed as desired.  */
  fenv_t env;
  fegetenv (&env);
  if (((FE_INVALID << 8) & ~env) != 0)
    return -1;
  #endif

  return 0;
}

/* But it does not work on RISC-V.  That's because the fcsr register has only
   bits for floating-point exception status, but no bits for trapping
   floating-point exceptions.  */
# if defined __riscv
#  undef HAVE_FPE_TRAPPING
# endif

#elif HAVE_FPSETMASK
/* FreeBSD ≥ 6.0, NetBSD ≥ 1.4, OpenBSD ≥ 3.1, IRIX, Solaris, Minix ≥ 3.2.  */

# include <ieeefp.h>
  /* The type is called 'fp_except_t' on FreeBSD, but 'fp_except' on
     all other systems.  */
# if !defined __FreeBSD__
#  define fp_except_t fp_except
# endif

static int
sigfpe_on_invalid ()
{
  /* Clear FE_INVALID exceptions from past operations.  */
  feclearexcept (FE_INVALID);

  fpsetmask (fpgetmask () | FP_X_INV);

  #if defined __arm__ || defined __aarch64__
  /* Test whether the CPU supports the request.  */
  if ((fpgetmask () & ~FP_X_INV) == 0)
    return -1;
  #endif

  return 0;
}

#elif HAVE_FESETTRAPENABLE
/* HP-UX, QNX */

# include <fenv.h>

static int
sigfpe_on_invalid ()
{
  /* Clear FE_INVALID exceptions from past operations.  */
  feclearexcept (FE_INVALID);

  fesettrapenable (fegettrapenable () | FE_INVALID);

  return 0;
}

#elif defined _AIX
/* AIX */

# include <fptrap.h>

static int
sigfpe_on_invalid ()
{
  /* Clear FE_INVALID exceptions from past operations.  */
  feclearexcept (FE_INVALID);

  /* Enable precise trapping mode.
     Documentation: <https://www.ibm.com/docs/en/aix/7.3?topic=f-fp-trap-subroutine>  */
  fp_trap (FP_TRAP_SYNC);
  /* Documentation: <https://www.ibm.com/docs/en/aix/7.3?topic=f-fp-any-enable-fp-is-enabled-fp-enable-all-fp-enable-fp-disable-all-fp-disable-subroutine>  */
  fp_enable (TRP_INVALID);

  return 0;
}

#elif __MINGW32__ && defined __x86_64__
/* mingw/x86_64 */

# include <fenv.h>

static int
sigfpe_on_invalid ()
{
  /* Clear FE_INVALID exceptions from past operations.  */
  feclearexcept (FE_INVALID);

  /* An FE_INVALID exception shall trigger a SIGFPE signal.
     fctrl bits 5..2,0 indicate which floating-point exceptions shall, when
     occurring in the 387 compatible floating-point unit, trigger a trap rather
     than merely set the corresponding bit in the fstat register.
     mxcsr bits 12..9,7 indicate which floating-point exceptions shall, when
     occurring in the SSE registers floating-point unit, trigger a trap rather
     than merely set the corresponding bit in the lower part of the mxcsr
     register.  */
  /* mingw's _controlfp_s implementation
     <https://github.com/mingw-w64/mingw-w64/blob/master/mingw-w64-crt/secapi/_controlfp_s.c>
     is broken.  This code here works.  */
  {
    unsigned short fctrl, orig_fctrl;
    unsigned int mxcsr, orig_mxcsr;

    __asm__ __volatile__ ("fstcw %0" : "=m" (*&fctrl));
    __asm__ __volatile__ ("stmxcsr %0" : "=m" (*&mxcsr));
    orig_fctrl = fctrl;
    orig_mxcsr = mxcsr;
    fctrl &= ~FE_INVALID;
    mxcsr &= ~(FE_INVALID << 7);
    if (!(fctrl == orig_fctrl && mxcsr == orig_mxcsr))
      {
        __asm__ __volatile__ ("fldcw %0" : : "m" (*&fctrl));
        __asm__ __volatile__ ("ldmxcsr %0" : : "m" (*&mxcsr));
      }
  }

  return 0;
}

#elif defined _WIN32 && !defined __CYGWIN__
/* native Windows */

# include <float.h>

static int
sigfpe_on_invalid ()
{
  /* Documentation:
     <https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/controlfp-s>  */
  unsigned int control;
  if (_controlfp_s (&control, 0, 0) == 0)
    if (_controlfp_s (&control, control & ~_EM_INVALID, _MCW_EM) == 0)
      return 0;

  return -1;
}

#elif MUSL_LIBC && defined __x86_64__
/* musl libc/x86_64 */

# include <fenv.h>

static int
sigfpe_on_invalid ()
{
  fenv_t env, orig_env;
  fegetenv (&env);
  orig_env = env;

  /* Clear FE_INVALID exceptions from past operations.
     fstat bits 5..2,0 indicate which floating-point exceptions have occurred
     since the respective bit was last set to zero, in the 387 compatible
     floating-point unit.
     mxcsr bits 5..2,0 indicate which floating-point exceptions have occurred
     since the respective bit was last set to zero, in the SSE registers
     floating-point unit.  */
  env.__status_word &= ~FE_INVALID;
  env.__mxcsr &= ~FE_INVALID;
  /* An FE_INVALID exception shall trigger a SIGFPE signal.
     fctrl bits 5..2,0 indicate which floating-point exceptions shall, when
     occurring in the 387 compatible floating-point unit, trigger a trap rather
     than merely set the corresponding bit in the fstat register.
     mxcsr bits 12..9,7 indicate which floating-point exceptions shall, when
     occurring in the SSE registers floating-point unit, trigger a trap rather
     than merely set the corresponding bit in the lower part of the mxcsr
     register.  */
  env.__control_word &= ~FE_INVALID;
  env.__mxcsr &= ~(FE_INVALID << 7);

  if (!(env.__control_word == orig_env.__control_word
        && env.__status_word == orig_env.__status_word
        && env.__mxcsr == orig_env.__mxcsr))
    fesetenv (&env);

  return 0;
}

#elif MUSL_LIBC && defined __i386__
/* musl libc/x86 */

# include <fenv.h>

static int
sigfpe_on_invalid ()
{
  fenv_t env, orig_env;
  fegetenv (&env);
  orig_env = env;

  /* The mxcsr register exists, like for x86_64, but only if the CPU supports
     SSE instructions.  Here it is not part of fenv_t.  Instead, it is
     transparently handled by the fegetenv and fesetenv functions.  */

  /* Clear FE_INVALID exceptions from past operations.
     fstat bits 5..2,0 indicate which floating-point exceptions have occurred
     since the respective bit was last set to zero, in the 387 compatible
     floating-point unit.  */
  env.__status_word &= ~FE_INVALID;
  /* An FE_INVALID exception shall trigger a SIGFPE signal.
     fctrl bits 5..2,0 indicate which floating-point exceptions shall, when
     occurring in the 387 compatible floating-point unit, trigger a trap rather
     than merely set the corresponding bit in the fstat register.  */
  env.__control_word &= ~FE_INVALID;

  if (!(env.__control_word == orig_env.__control_word
        && env.__status_word == orig_env.__status_word))
    fesetenv (&env);

  return 0;
}

#elif MUSL_LIBC && defined __aarch64__
/* musl libc/arm64 */

# include <fenv.h>

static int
sigfpe_on_invalid ()
{
  fenv_t env, orig_env;
  fegetenv (&env);
  orig_env = env;

  /* Clear FE_INVALID exceptions from past operations.
     fpsr bits 4..0 indicate which floating-point exceptions have occurred
     since the respective bit was last set to zero.  */
  env.__fpsr &= ~FE_INVALID;
  /* An FE_INVALID exception shall trigger a SIGFPE signal.
     fpcr bits 12..8 indicate which floating-point exceptions shall, when
     occurring, trigger a trap rather than merely set the corresponding bit
     in the fpsr register.  */
  env.__fpcr |= (FE_INVALID << 8);

  if (!(env.__fpsr == orig_env.__fpsr && env.__fpcr == orig_env.__fpcr))
    {
      fesetenv (&env);
      /* Test whether __fpcr was actually changed as desired.  */
      fenv_t new_env;
      fegetenv (&new_env);
      if (new_env.__fpcr != env.__fpcr)
        return -1;
    }

  return 0;
}

#elif MUSL_LIBC && defined __arm__
/* musl libc/arm */

# include <fenv.h>

static int
sigfpe_on_invalid ()
{
  fenv_t env;
  unsigned int orig_fpscr;
  fegetenv (&env);
  orig_fpscr = env.__cw;

  /* Clear FE_INVALID exceptions from past operations.
     fpscr bits 4..0 indicate which floating-point exceptions have occurred
     since the respective bit was last set to zero.  */
  env.__cw &= ~FE_INVALID;
  /* An FE_INVALID exception shall trigger a SIGFPE signal.
     fpscr bits 12..8 indicate which floating-point exceptions shall, when
     occurring, trigger a trap rather than merely set the corresponding bit
     in the fpscr register.  */
  env.__cw |= (FE_INVALID << 8);

  if (!(env.__cw == orig_fpscr))
    {
      fesetenv (&env);
      /* Test whether fpscr was actually changed as desired.  */
      fenv_t new_env;
      fegetenv (&new_env);
      if (new_env.__cw != env.__cw)
        return -1;
    }

  return 0;
}

#elif MUSL_LIBC && defined __m68k__
/* musl libc/m68k */

# include <fenv.h>

static int
sigfpe_on_invalid ()
{
  fenv_t env, orig_env;
  fegetenv (&env);
  orig_env = env;

  /* Clear FE_INVALID exceptions from past operations.
     fpsr bits 7..3 indicate which floating-point exceptions have occurred
     since the respective bit was last set to zero.  */
  env.__status_register &= ~FE_INVALID;
  /* An FE_INVALID exception shall trigger a SIGFPE signal.
     fpcr bits 15..8 indicate which floating-point exceptions shall, when
     occurring, trigger a trap rather than merely set the corresponding bit
     in the fpsr register:
       - bit 15: branch/set on unordered
       - bit 14: signaling not-a-number
       - bit 13: operand error
       - bit 12: overflow
       - bit 11: underflow
       - bit 10: divide by zero
       - bit 9:  inexact operation
       - bit 8:  inexact decimal input
     Although (FE_INVALID << 6) has bit 13 set, we need to set bit 14 and
     bit 13.  */
  env.__control_register |= (1U << 14) | (FE_INVALID << 6);

  if (!(env.__status_register == orig_env.__status_register
        && env.__control_register == orig_env.__control_register))
    fesetenv (&env);

  return 0;
}

#elif MUSL_LIBC && defined __mips__
/* musl libc/mips64,mipsn32,mips32 */

# include <fenv.h>

static int
sigfpe_on_invalid ()
{
  fenv_t env, orig_env;
  fegetenv (&env);
  orig_env = env;

  /* Clear FE_INVALID exceptions from past operations.
     fcsr bits 6..2 indicate which floating-point exceptions have occurred
     since the respective bit was last set to zero.
     fcsr bits 17..12 indicate which floating-point exceptions have occurred
     in the most recent instruction.  */
  env.__cw &= ~((FE_INVALID << 10) | FE_INVALID);
  /* An FE_INVALID exception shall trigger a SIGFPE signal.
     fcsr bits 11..7 indicate which floating-point exceptions shall, when
     occurring, trigger a trap rather than merely set the corresponding bit
     in the fcsr register.  */
  env.__cw |= (FE_INVALID << 5);

  if (!(env.__cw == orig_env.__cw))
    fesetenv (&env);

  return 0;
}

#elif MUSL_LIBC && (defined __powerpc__ || defined __powerpc64__)
/* musl libc/powerpc64,powerpc */

# include <fenv.h>
# include <sys/prctl.h>

static int
sigfpe_on_invalid ()
{
  /* fenv_t is a 'double'.  */
  union { unsigned long long u; fenv_t f; } env, orig_env;
  fegetenv (&env.f);
  orig_env = env;

  /* Clear FE_INVALID exceptions from past operations.
     fpscr bits 28..25 indicate which floating-point exceptions, other than
     FE_INVALID, have occurred since the respective bit was last set to zero.
     fpscr bits 24..19, 10..8 do the same thing, for various kinds of Invalid
     Operation.  fpscr bit 29 is the summary (the OR) of all these bits.
     Instead of clearing FE_INVALID (= bit 29), we need to clear these
     individual bits.  */
  env.u &= ~FE_ALL_INVALID; /* not ~FE_INVALID ! */
  /* An FE_INVALID exception shall trigger a SIGFPE signal.
     fpscr bits 7..3 indicate which floating-point exceptions shall, when
     occurring, trigger a trap rather than merely set the corresponding bit
     in the fpscr register.  */
  env.u |= (FE_INVALID >> 22);

  if (!(env.u == orig_env.u))
    {
      /* Enable precise trapping mode.  */
      prctl (PR_SET_FPEXC, PR_FP_EXC_PRECISE);

      fesetenv (&env.f);
    }

  return 0;
}

#elif MUSL_LIBC && (defined __s390__ || defined __s390x__)
/* musl libc/s390x,s390 */

# include <fenv.h>

static int
sigfpe_on_invalid ()
{
  fenv_t env, orig_env;
  fegetenv (&env);
  orig_env = env;

  /* Clear FE_INVALID exceptions from past operations.
     fpc bits 23..19 indicate which floating-point exceptions have occurred
     since the respective bit was last set to zero.
     fpc bits 15..11 are part of the "data exception code" (DXC) and have a
     similar meaning if bits 9..8 are both zero.  */
  env &= ~FE_INVALID;
  if ((env & 0x00000300) == 0)
    env &= ~(FE_INVALID >> 8);
  /* An FE_INVALID exception shall trigger a SIGFPE signal.
     fpc bits 31..27 indicate which floating-point exceptions shall, when
     occurring, trigger a trap rather than merely set the corresponding bit
     in the fpc register.  */
  env |= ((unsigned int) FE_INVALID << 8);

  if (!(env == orig_env))
    fesetenv (&env);

  return 0;
}

#elif MUSL_LIBC && defined __sh__
/* musl libc/sh4 */

# include <fenv.h>

static int
sigfpe_on_invalid ()
{
  fenv_t env, orig_env;
  fegetenv (&env);
  orig_env = env;

  /* Clear FE_INVALID exceptions from past operations.
     fpscr bits 6..2 indicate which floating-point exceptions have occurred
     since the respective bit was last set to zero.  */
  env.__cw &= ~FE_INVALID;
  /* An FE_INVALID exception shall trigger a SIGFPE signal.
     fpscr bits 11..7 indicate which floating-point exceptions shall, when
     occurring, trigger a trap rather than merely set the corresponding bit
     in the fpscr register.  */
  env.__cw |= (FE_INVALID << 5);

  if (!(env.__cw == orig_env.__cw))
    fesetenv (&env);

  return 0;
}

#elif (defined __APPLE__ && defined __MACH__) && (defined __i386__ || defined __x86_64__)
/* Mac OS X/i386,x86_64 */

# include <fenv.h>

static int
sigfpe_on_invalid ()
{
  fenv_t env, orig_env;
  fegetenv (&env);
  orig_env = env;

  /* Clear FE_INVALID exceptions from past operations.
     fstat bits 5..2,0 indicate which floating-point exceptions have occurred
     since the respective bit was last set to zero, in the 387 compatible
     floating-point unit.
     mxcsr bits 5..2,0 indicate which floating-point exceptions have occurred
     since the respective bit was last set to zero, in the SSE registers
     floating-point unit.  */
  env.__status &= ~FE_INVALID;
  env.__mxcsr &= ~FE_INVALID;
  /* An FE_INVALID exception shall trigger a SIGFPE signal.
     fctrl bits 5..2,0 indicate which floating-point exceptions shall, when
     occurring in the 387 compatible floating-point unit, trigger a trap rather
     than merely set the corresponding bit in the fstat register.
     mxcsr bits 12..9,7 indicate which floating-point exceptions shall, when
     occurring in the SSE registers floating-point unit, trigger a trap rather
     than merely set the corresponding bit in the lower part of the mxcsr
     register.  */
  env.__control &= ~FE_INVALID;
  env.__mxcsr &= ~(FE_INVALID << 7);

  if (!(env.__control == orig_env.__control
        && env.__status == orig_env.__status
        && env.__mxcsr == orig_env.__mxcsr))
    fesetenv (&env);

  return 0;
}

#elif (defined __APPLE__ && defined __MACH__) && defined __aarch64__
/* macOS/arm64 */

# include <fenv.h>

static int
sigfpe_on_invalid ()
{
  fenv_t env, orig_env;
  fegetenv (&env);
  orig_env = env;

  /* Clear FE_INVALID exceptions from past operations.
     fpsr bits 4..0 indicate which floating-point exceptions have occurred
     since the respective bit was last set to zero.  */
  env.__fpsr &= ~FE_INVALID;
  /* An FE_INVALID exception shall trigger a SIGILL signal.
     fpcr bits 12..8 indicate which floating-point exceptions shall, when
     occurring, trigger a trap rather than merely set the corresponding bit
     in the fpsr register.  */
  env.__fpcr |= (FE_INVALID << 8); /* __fpcr_trap_invalid */

  if (!(env.__fpsr == orig_env.__fpsr && env.__fpcr == orig_env.__fpcr))
    fesetenv (&env);

  return 0;
}

#elif (defined __APPLE__ && defined __MACH__) && defined __arm__
/* macOS/arm */

# include <fenv.h>

static int
sigfpe_on_invalid ()
{
  fenv_t env;
  unsigned int orig_fpscr;
  fegetenv (&env);
  orig_fpscr = env.__fpscr;

  /* Clear FE_INVALID exceptions from past operations.
     fpscr bits 4..0 indicate which floating-point exceptions have occurred
     since the respective bit was last set to zero.  */
  env.__fpscr &= ~FE_INVALID;
  /* An FE_INVALID exception shall trigger a signal.
     fpscr bits 12..8 indicate which floating-point exceptions shall, when
     occurring, trigger a trap rather than merely set the corresponding bit
     in the fpscr register.  */
  env.__fpscr |= (FE_INVALID << 8); /* __fpscr_trap_invalid */

  if (!(env.__fpscr == orig_fpscr))
    fesetenv (&env);

  return 0;
}

#else

# undef HAVE_FPE_TRAPPING

#endif
