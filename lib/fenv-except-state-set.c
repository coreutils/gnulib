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
fesetexceptflag (fexcept_t const *saved_flags, int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;

  unsigned int desired_flags = (unsigned int) *saved_flags;

  exceptions = exceptions_to_x86hardware (exceptions);
  desired_flags = exceptions_to_x86hardware (desired_flags);

#  if defined _MSC_VER

  /* Modify the flags in the SSE unit.  */
  unsigned int mxcsr, orig_mxcsr;
  _FPU_GETSSECW (orig_mxcsr);
  mxcsr = orig_mxcsr ^ ((orig_mxcsr ^ desired_flags) & exceptions);
  if (mxcsr != orig_mxcsr)
    _FPU_SETSSECW (mxcsr);

#  else

  /* The flags can be set in the 387 unit or in the SSE unit.
     When we need to clear a flag, we need to do so in both units,
     due to the way fetestexcept() is implemented.
     When we need to set a flag, it is sufficient to do it in the SSE unit,
     because that is guaranteed to not trap.  However, on i386 CPUs that have
     only a 387 unit, set the flags in the 387, as long as this cannot trap.  */

  if (CPU_HAS_SSE ())
    {
      /* Modify the flags in the SSE unit.  */
      unsigned int mxcsr, orig_mxcsr;
      _FPU_GETSSECW (orig_mxcsr);
      mxcsr = orig_mxcsr ^ ((orig_mxcsr ^ desired_flags) & exceptions);
      if (mxcsr != orig_mxcsr)
        _FPU_SETSSECW (mxcsr);

      /* Modify the flags in the 387 unit, but only by clearing bits, not by
         setting bits.  */
      x86_387_fenv_t env;
      __asm__ __volatile__ ("fnstenv %0" : "=m" (*&env));
      /* Note: fnstenv masks all floating-point exceptions until the fldenv
         below.  */
      env.__status_word &= ~ (exceptions & ~desired_flags);
      __asm__ __volatile__ ("fldenv %0" : : "m" (*&env));
    }
#   if !(defined __x86_64__ || defined _M_X64)
  else
    {
      /* Modify the flags in the 387 unit.  */
      x86_387_fenv_t env;
      __asm__ __volatile__ ("fnstenv %0" : "=m" (*&env));
      /* Note: fnstenv masks all floating-point exceptions until the fldenv
         or fldcw below.  */
      env.__status_word ^= ((env.__status_word ^ desired_flags) & exceptions);
      if ((~env.__control_word) & env.__status_word & exceptions)
        {
          /* Setting the exception flags may trigger a trap (at the next
             floating-point instruction, but that does not matter).
             ISO C 23 § 7.6.4.5 does not allow it.  */
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

/* On Linux, NetBSD, and Android, fexcept_t is binary-equivalent to
   an 'unsigned int'.
   On macOS, fexcept_t is binary-equivalent to an 'unsigned short'.
   On FreeBSD and OpenBSD, fexcept_t is binary-equivalent to an 'unsigned long'.
   A simple C cast does the necessary conversion.  */

int
fesetexceptflag (fexcept_t const *saved_flags, int exceptions)
{
  unsigned long desired_flags = (unsigned long) *saved_flags;
  unsigned long fpsr, orig_fpsr;
  _FPU_GETFPSR (orig_fpsr);
  fpsr = orig_fpsr ^ ((orig_fpsr ^ desired_flags) & FE_ALL_EXCEPT & exceptions);
  if (fpsr != orig_fpsr)
    _FPU_SETFPSR (fpsr);
  /* Possibly some new exception flags have been set.  But just storing them
     does not cause a trap to be executed (when traps are enabled).  */
  return 0;
}

# elif defined __arm__

/* On all OSes, fexcept_t is binary-equivalent to an 'unsigned int'.  */

int
fesetexceptflag (fexcept_t const *saved_flags, int exceptions)
{
#  ifdef __SOFTFP__
  if (exceptions != 0)
    return -1;
#  else
  unsigned int desired_flags = (unsigned int) *saved_flags;
  unsigned int fpscr, orig_fpscr;
  _FPU_GETCW (orig_fpscr);
  fpscr = orig_fpscr ^ ((orig_fpscr ^ desired_flags) & FE_ALL_EXCEPT & exceptions);
  if (fpscr != orig_fpscr)
    _FPU_SETCW (fpscr);
#  endif
  return 0;
}

# elif defined __alpha

/* On all OSes except NetBSD and OpenBSD, fexcept_t is binary-equivalent to
   an 'unsigned long'.
   On NetBSD, it is equivalent to an 'unsigned short'.
   On OpenBSD, it is equivalent to an 'unsigned int'.
   A simple C cast does the necessary conversion.  */

int
fesetexceptflag (fexcept_t const *saved_flags, int exceptions)
{
  unsigned long desired_flags = (unsigned long) *saved_flags;
  unsigned long swcr, orig_swcr;
  orig_swcr = __ieee_get_fp_control ();
  swcr = orig_swcr ^ ((orig_swcr ^ desired_flags) & FE_ALL_EXCEPT & exceptions);
  if (swcr != orig_swcr)
    __ieee_set_fp_control (swcr);
  /* Possibly some new exception flags have been set.  But just storing them
     does not cause a trap to be executed (when traps are enabled).  */
  return 0;
}

# elif defined __hppa

/* On all OSes, fexcept_t is binary-equivalent to an 'unsigned int'.  */

int
fesetexceptflag (fexcept_t const *saved_flags, int exceptions)
{
  unsigned int desired_flags = (unsigned int) *saved_flags;
  union { unsigned long long fpreg; unsigned int halfreg[2]; } s;
  /* Get the current status word. */
  __asm__ __volatile__ ("fstd %%fr0,0(%1)" : "=m" (s.fpreg) : "r" (&s.fpreg) : "%r0");
  unsigned int old_halfreg0 = s.halfreg[0];
  /* Modify all the relevant bits. */
  s.halfreg[0] ^= (s.halfreg[0] ^ (desired_flags << 27)) & ((FE_ALL_EXCEPT & exceptions) << 27);
  if (s.halfreg[0] != old_halfreg0)
    {
      /* Store the new status word.  */
      __asm__ __volatile__ ("fldd 0(%0),%%fr0" : : "r" (&s.fpreg), "m" (s.fpreg) : "%r0");
    }
  /* Possibly some new exception flags have been set.  But just storing them
     does not cause a trap to be executed (when traps are enabled).  */
  return 0;
}

# elif defined __ia64__

/* On all OSes except NetBSD, fexcept_t is binary-equivalent to
   an 'unsigned long'.
   On NetBSD, it is equivalent to an 'unsigned short'.
   A simple C cast does the necessary conversion.  */

int
fesetexceptflag (fexcept_t const *saved_flags, int exceptions)
{
  unsigned long desired_flags = (unsigned long) *saved_flags;
  unsigned long fpsr, orig_fpsr;
  _FPU_GETCW (orig_fpsr);
  fpsr = orig_fpsr ^ ((orig_fpsr ^ (desired_flags << 13)) & ((FE_ALL_EXCEPT & exceptions) << 13));
  if (fpsr != orig_fpsr)
    _FPU_SETCW (fpsr);
  /* Possibly some new exception flags have been set.  But just storing them
     does not cause a trap to be executed (when traps are enabled).  */
  return 0;
}

# elif defined __m68k__

/* On all OSes, fexcept_t is binary-equivalent to an 'unsigned int'.  */

int
fesetexceptflag (fexcept_t const *saved_flags, int exceptions)
{
  unsigned int desired_flags = (unsigned int) *saved_flags;
  unsigned int fpsr, orig_fpsr;
  _FPU_GETFPSR (orig_fpsr);
  fpsr = orig_fpsr ^ ((orig_fpsr ^ desired_flags) & FE_ALL_EXCEPT & exceptions);
  if (fpsr != orig_fpsr)
    _FPU_SETFPSR (fpsr);
  /* Possibly some new exception flags have been set.  But just storing them
     does not cause a trap to be executed (when traps are enabled).  */
  return 0;
}

# elif defined __mips__

/* On all OSes except NetBSD and OpenBSD, fexcept_t is binary-equivalent to
   an 'unsigned short'.
   On NetBSD and OpenBSD, it is equivalent to an 'unsigned int'.
   A simple C cast does the necessary conversion.  */

int
fesetexceptflag (fexcept_t const *saved_flags, int exceptions)
{
  unsigned int desired_flags = (unsigned int) *saved_flags;
  unsigned int fcsr, orig_fcsr;
  _FPU_GETCW (orig_fcsr);
  fcsr = orig_fcsr ^ ((orig_fcsr ^ desired_flags) & FE_ALL_EXCEPT & exceptions);
  if (fcsr != orig_fcsr)
    _FPU_SETCW (fcsr);
  return 0;
}

# elif defined __loongarch__

/* On all OSes, fexcept_t is binary-equivalent to an 'unsigned int'.  */

int
fesetexceptflag (fexcept_t const *saved_flags, int exceptions)
{
  unsigned int desired_flags = (unsigned int) *saved_flags;
  unsigned int fcsr, orig_fcsr;
  _FPU_GETCW (orig_fcsr);
  fcsr = orig_fcsr ^ ((orig_fcsr ^ desired_flags) & FE_ALL_EXCEPT & exceptions);
  if (fcsr != orig_fcsr)
    _FPU_SETCW (fcsr);
  /* Possibly some new exception flags have been set.  But just storing them
     does not cause a trap to be executed (when traps are enabled).  */
  return 0;
}

# elif defined _ARCH_PPC

/* On all OSes, fexcept_t is binary-equivalent to an 'unsigned int'.  */

int
fesetexceptflag (fexcept_t const *saved_flags, int exceptions)
{
  /* The hardware does not support setting an exception flag without triggering
     a trap, except through the "Ignore Exceptions Mode", bits FE0 and FE1 of
     the MSR register set to zero, that can be obtained through a system call:
       - On Linux and NetBSD: prctl (PR_SET_FPEXC, PR_FP_EXC_DISABLED);
       - On AIX: fp_trap (FP_TRAP_OFF);
     But that is not what we need here, as it would have a persistent effect on
     the thread.  */
  exceptions &= FE_ALL_EXCEPT;

  unsigned int desired_flags = (unsigned int) *saved_flags;
  desired_flags &= exceptions;

  union { unsigned long long u; double f; } memenv, orig_memenv;
  _FPU_GETCW_AS_DOUBLE (memenv.f);
  orig_memenv = memenv;

  /* Instead of clearing FE_INVALID (= bit 29), we need to clear the
     individual bits.  */
  memenv.u &= ~ (exceptions & FE_INVALID
                 ? (exceptions & ~FE_INVALID) | 0x01F80700U
                 : exceptions);
  /* Instead of setting FE_INVALID (= bit 29), we need to set one of the
     individual bits: bit 10 or, if that does not work, bit 24.  */
  memenv.u |= (desired_flags & FE_INVALID
               ? (desired_flags & ~FE_INVALID) | (1U << 10)
               : desired_flags);

  if (!(memenv.u == orig_memenv.u))
    {
      if (memenv.u & (exceptions >> 22))
        {
          /* Setting the exception flags may trigger a trap.
             ISO C 23 § 7.6.4.5 does not allow it.  */
          return -1;
        }
      _FPU_SETCW_AS_DOUBLE (memenv.f);
      if (desired_flags & FE_INVALID)
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

/* On all OSes except FreeBSD, fexcept_t is binary-equivalent to
   an 'unsigned int'.
   On FreeBSD, it is equivalent to an 'unsigned long'.
   A simple C cast does the necessary conversion.  */

int
fesetexceptflag (fexcept_t const *saved_flags, int exceptions)
{
#  if 1 /* both should be equivalent */
  exceptions &= FE_ALL_EXCEPT;

  unsigned int desired_flags = (unsigned int) *saved_flags;
  desired_flags &= exceptions;

  __asm__ __volatile__ ("csrc fflags, %0" : : "r" (exceptions));
  __asm__ __volatile__ ("csrs fflags, %0" : : "r" (desired_flags));
#  else
  unsigned int desired_flags = (unsigned int) *saved_flags;
  unsigned int fcsr, orig_fcsr;
  __asm__ __volatile__ ("frflags %0" : "=r" (orig_fcsr)); /* same as "csrr %0, fflags" */
  fcsr = orig_fcsr ^ ((orig_fcsr ^ desired_flags) & FE_ALL_EXCEPT & exceptions);
  if (fcsr != orig_fcsr)
    __asm__ __volatile__ ("fsflags %0" : : "r" (fcsr)); /* same as "csrw fflags, %0" */
#  endif

  return 0;
}

# elif defined __s390__ || defined __s390x__

/* On all OSes, fexcept_t is binary-equivalent to an 'unsigned int'.  */

int
fesetexceptflag (fexcept_t const *saved_flags, int exceptions)
{
  unsigned int desired_flags = (unsigned int) *saved_flags;
  unsigned int fpc, orig_fpc;
  _FPU_GETCW (orig_fpc);
#  if FE_INEXACT == 8 /* glibc compatible FE_* values */
  fpc = orig_fpc ^ ((orig_fpc ^ (desired_flags << 16)) & ((FE_ALL_EXCEPT & exceptions) << 16));
  if ((fpc & 0x00000300) == 0)
    /* Clear the corresponding bits of the "data exception code".  */
    fpc &= ~((FE_ALL_EXCEPT & exceptions) << 8);
#  else /* musl libc compatible FE_* values */
  fpc = orig_fpc ^ ((orig_fpc ^ desired_flags) & FE_ALL_EXCEPT & exceptions);
  if ((fpc & 0x00000300) == 0)
    /* Clear the corresponding bits of the "data exception code".  */
    fpc &= ~((FE_ALL_EXCEPT & exceptions) >> 8);
#  endif
  if (fpc != orig_fpc)
    _FPU_SETCW (fpc);
  /* Possibly some new exception flags have been set.  But just storing them
     does not cause a trap to be executed (when traps are enabled).  */
  return 0;
}

# elif defined __sh__

/* On glibc, fexcept_t is binary-equivalent to an 'unsigned short'.
   On all other OSes, fexcept_t is binary-equivalent to an 'unsigned int'.
   A simple C cast does the necessary conversion.  */

int
fesetexceptflag (fexcept_t const *saved_flags, int exceptions)
{
  unsigned int desired_flags = (unsigned int) *saved_flags;
  unsigned int fpscr, orig_fpscr;
  _FPU_GETCW (orig_fpscr);
  fpscr = orig_fpscr ^ ((orig_fpscr ^ desired_flags) & FE_ALL_EXCEPT & exceptions);
  if (fpscr != orig_fpscr)
    _FPU_SETCW (fpscr);
  /* Possibly some new exception flags have been set.  But just storing them
     does not cause a trap to be executed (when traps are enabled).  */
  return 0;
}

# elif defined __sparc

/* On all OSes except Solaris, fexcept_t is binary-equivalent to an 'unsigned long'.
   On Solaris, fexcept_t is an 'int'.
   A simple C cast does the necessary conversion.  */

int
fesetexceptflag (fexcept_t const *saved_flags, int exceptions)
{
  unsigned long desired_flags = (unsigned long) *saved_flags;
  unsigned long fsr, orig_fsr;
  _FPU_GETCW (orig_fsr);
#  if FE_INEXACT == 32 /* glibc compatible FE_* values */
  fsr = orig_fsr ^ ((orig_fsr ^ desired_flags) & FE_ALL_EXCEPT & exceptions);
#  else /* Solaris compatible FE_* values */
  fsr = orig_fsr ^ ((orig_fsr ^ (desired_flags << 5)) & ((FE_ALL_EXCEPT & exceptions) << 5));
#  endif
  if (fsr != orig_fsr)
    _FPU_SETCW (fsr);
  /* Possibly some new exception flags have been set.  But just storing them
     does not cause a trap to be executed (when traps are enabled).  */
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

# if (defined __sun || __GLIBC__ >= 2) && ((defined __x86_64__ || defined _M_X64) || (defined __i386 || defined _M_IX86)) && defined __SUNPRO_C
/* Solaris/i386, Solaris/x86_64, glibc/i386, glibc/x86_64, with SunPRO C.  */

/* On these Solaris platforms, fpsetsticky cannot be used here, because it may
   generate traps (since fpsetsticky calls _putsw, which modifies the control
   word of the 387 unit).  Instead, we need to modify only the flags in the SSE
   unit.  */

/* Accessors for the mxcsr register.  Fortunately, the SunPRO C compiler
   supports a poor form of 'asm'.  */

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

#  if __GLIBC__ >= 2
/* Clears flags in the 387 unit.  */
static void
mask387cw (unsigned short mask)
{
#   if defined __x86_64__ || defined _M_X64
  asm ("fnstenv -32(%rsp)");
  asm ("andw %di,-28(%rsp)");
  asm ("fldenv -32(%rsp)");
#   else
  /* The compiler generates a stack frame.  Therefore the first argument is in
     8(%ebp), not in 4(%esp).  */
  asm ("movl 8(%ebp),%eax");
  asm ("fnstenv -32(%esp)");
  asm ("andw %ax,-28(%esp)");
  asm ("fldenv -32(%esp)");
#   endif
}
#  endif

int
fesetexceptflag (fexcept_t const *saved_flags, int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;
  unsigned int desired_flags = (unsigned int) *saved_flags;

  /* Modify the flags in the SSE unit.  */
  unsigned int mxcsr, orig_mxcsr;
  getssecw (&orig_mxcsr);
  mxcsr = orig_mxcsr ^ ((orig_mxcsr ^ desired_flags) & exceptions);
  if (mxcsr != orig_mxcsr)
    setssecw (&mxcsr);

#  if __GLIBC__ >= 2
  /* Modify the flags in the 387 unit, but only by clearing bits, not by
     setting bits.  */
  mask387cw (~ (exceptions & ~desired_flags));
#  endif

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
fesetexceptflag (fexcept_t const *saved_flags, int exceptions)
{
  unsigned long desired_flags = (unsigned long) *saved_flags;
  fp_except_t flags, orig_flags;
  orig_flags = fpgetsticky ();
  flags = orig_flags ^ ((orig_fsr ^ desired_flags) & FE_ALL_EXCEPT & exceptions);
  if (flags != orig_flags)
    fpsetsticky (flags);
  /* Possibly some new exception flags have been set.  But just storing them
     does not cause a trap to be executed (when traps are enabled).  */
  return 0;
}

# elif defined _AIX && defined _ARCH_PPC /* AIX */

#  include <float.h>
#  include <fpxcp.h>

#  include <fptrap.h>

/* Documentation:
   <https://www.ibm.com/docs/en/aix/7.3?topic=f-fp-clr-flag-fp-set-flag-fp-read-flag-fp-swap-flag-subroutine>  */

int
fesetexceptflag (fexcept_t const *saved_flags, int exceptions)
{
  exceptions &= FE_ALL_EXCEPT;

  unsigned int desired_flags = (unsigned int) *saved_flags;
  int exceptions_to_clear = exceptions & ~desired_flags;
  int exceptions_to_set = exceptions & desired_flags;

  fpflag_t orig_flags = fp_read_flag ();
  /* In addition to clearing FE_INVALID (= bit 29), we also need to clear the
     individual bits.  */
  fpflag_t f_to_clear =
    exceptions_to_fpflag (exceptions_to_clear)
    | (exceptions_to_clear & FE_INVALID ? 0x01F80700U : 0);
  /* Instead of setting FE_INVALID (= bit 29), we need to set one of the
     individual bits: bit 10 or, if that does not work, bit 24.  */
  fpflag_t f_to_set =
    (exceptions_to_set & FE_INVALID
     ? exceptions_to_fpflag (exceptions_to_set & ~FE_INVALID) | (1U << 10)
     : exceptions_to_fpflag (exceptions_to_set));
  if (f_to_clear != 0)
    fp_clr_flag (f_to_clear);
  if (f_to_set != 0)
    {
      if ((fegetexcept_impl () & exceptions) != 0)
        {
          /* Setting the exception flags may trigger a trap.
             ISO C 23 § 7.6.4.5 does not allow it.  */
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
fesetexceptflag (fexcept_t const *saved_flags, int exceptions)
{
  if (exceptions != 0)
    return -1;
  return 0;
}

#endif
