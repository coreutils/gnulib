/* Common definitions for the implementation of the various <fenv.h> modules.
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

/* Based on glibc/sysdeps/<cpu>/{fpu_control.h,fenv_private.h,fenv_libc.h}.  */

#if (defined __x86_64__ || defined _M_X64) || (defined __i386 || defined _M_IX86)

# if !(defined __x86_64__ || defined _M_X64)
#  if __GLIBC__ + (__GLIBC_MINOR__ >= 33) > 2
/* glibc >= 2.33 has an API that tells us whether the CPU has an SSE unit.  */
#   include <sys/platform/x86.h>
#  elif defined __sun
/* Solaris has a global variable that tells us whether the CPU has an SSE unit.  */
extern int _sse_hw;
#  endif
# endif

/* CPU_HAS_SSE ()  returns true if the CPU has an SSE unit.  */
# if defined __x86_64__ || defined _M_X64
#  define CPU_HAS_SSE() 1
# else
#  if __GLIBC__ + (__GLIBC_MINOR__ >= 33) > 2
#   define CPU_HAS_SSE() CPU_FEATURE_PRESENT (SSE)
#  elif defined __sun
#   define CPU_HAS_SSE() _sse_hw
#  else
/* Otherwise, we assume that the SSE unit is present.
   Only very old 32-bit processors, before Pentium 4, don't have it.
   Don't bother testing it, through a 'cpuid' instruction.  */
#   define CPU_HAS_SSE() 1
#  endif
# endif

/* fstat bits 5..2,0 indicate which floating-point exceptions have occurred
   in the 387 compatible floating-point unit since the respective bit was last
   set to zero.
   mxcsr bits 5..2,0 indicate which floating-point exceptions have occurred
   in the SSE floating-point unit since the respective bit was last set to
   zero.  */
/* fctrl bits 5..2,0 indicate which floating-point exceptions shall, when
   occurring in the 387 compatible floating-point unit, *not* trigger a trap
   rather than merely set the corresponding bit in the fstat register.
   mxcsr bits 12..9,7 indicate which floating-point exceptions shall, when
   occurring in the SSE floating-point unit, *not* trigger a trap rather
   than merely set the corresponding bit in the mxcsr register.  */

/* Macros that access the control word of the 387 unit, the so-called fctrl
   register.  */
# define _FPU_GETCW(cw) __asm__ __volatile__ ("fnstcw %0" : "=m" (*&cw))
# define _FPU_SETCW(cw) __asm__ __volatile__ ("fldcw %0" : : "m" (*&cw))

/* Macros that access the status word of the 387 unit, the so-called fstat
   register.  */
# define _FPU_GETSTAT(cw) __asm__ __volatile__ ("fnstsw %0" : "=m" (*&cw))

/* Macros that access the control and status word of the SSE unit, the mxcsr
   register.  */
# if defined __GNUC__ || defined __clang__
#  define _FPU_GETSSECW(cw) __asm__ __volatile__ ("stmxcsr %0" : "=m" (*&cw))
#  define _FPU_SETSSECW(cw) __asm__ __volatile__ ("ldmxcsr %0" : : "m" (*&cw))
# elif defined _MSC_VER
#  include <mmintrin.h>
/* Documentation:
   <https://learn.microsoft.com/en-us/cpp/intrinsics/x86-intrinsics-list>
   <https://learn.microsoft.com/en-us/cpp/intrinsics/x64-amd64-intrinsics-list>
   <https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html#text=_mm_getcsr&ig_expand=3548>
   <https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html#text=_mm_setcsr&ig_expand=5924>
 */
#  define _FPU_GETSSECW(cw) ((cw) = _mm_getcsr ())
#  define _FPU_SETSSECW(cw) _mm_setcsr (cw)
# endif

/* The floating-point environment of the 387 unit.  */
typedef struct
  {
    /* 7 32-bit words:  */
    unsigned short __control_word;      /* fctrl register */
    unsigned short __reserved1;
    unsigned short __status_word;       /* fstat register */
    unsigned short __reserved2;
    unsigned int more[5];
  }
x86_387_fenv_t;

# if defined _MSC_VER || (defined __MINGW32__ && FE_INVALID != 0x01)
/* The MSVC and mingw ≥ 13 header files have different values for the
   floating-point exceptions than all the other platforms.  Define some
   handy macros for conversion.  */
#  define exceptions_to_x86hardware(exceptions) \
     (  ((exceptions) & FE_INVALID   ? 0x01 : 0) \
      | ((exceptions) & FE_DIVBYZERO ? 0x04 : 0) \
      | ((exceptions) & FE_OVERFLOW  ? 0x08 : 0) \
      | ((exceptions) & FE_UNDERFLOW ? 0x10 : 0) \
      | ((exceptions) & FE_INEXACT   ? 0x20 : 0))
#  define x86hardware_to_exceptions(fstat) \
     (  ((fstat) & 0x01 ? FE_INVALID   : 0) \
      | ((fstat) & 0x04 ? FE_DIVBYZERO : 0) \
      | ((fstat) & 0x08 ? FE_OVERFLOW  : 0) \
      | ((fstat) & 0x10 ? FE_UNDERFLOW : 0) \
      | ((fstat) & 0x20 ? FE_INEXACT   : 0))
# else
#  define exceptions_to_x86hardware(exceptions) (exceptions)
#  define x86hardware_to_exceptions(fstat) (fstat)
# endif

/* When _MSC_VER is defined, the 387 compatible floating-point unit is *not*
   in use.  Only the SSE floating-point unit is used.  This can be inferred
   from two facts:
     - sizeof (long double) == sizeof (double).  That is, 'long double'
       values are just 'double' values and can be processed in the SSE unit.
     - After fegetenv (&env), the value of env._Fe_stat is *not* the fstat
       register of the 387 unit.  Rather, it is a artificial value.  In
       particular, (env._Fe_stat & 0x3f) is
       == x86hardware_to_exceptions (_FPU_GETSSECW () & 0x3f).  */

#elif defined __aarch64__ /* arm64 */

/* fpsr bits 4..0 indicate which floating-point exceptions have occurred
   since the respective bit was last set to zero.  */
/* fpcr bits 12..8 indicate which floating-point exceptions shall, when
   occurring, trigger a trap rather than merely set the corresponding bit
   in the fpsr register.  */

# if __GNUC__ >= 6 && !defined __clang__
#  define _FPU_GETCW(fpcr) (fpcr = __builtin_aarch64_get_fpcr ())
#  define _FPU_SETCW(fpcr) __builtin_aarch64_set_fpcr (fpcr)
# elif __clang_major__ >= 4
#  define _FPU_GETCW(fpcr) (fpcr = __builtin_arm_rsr ("fpcr"))
#  define _FPU_SETCW(fpcr) __builtin_arm_wsr ("fpcr", fpcr)
# else
#  define _FPU_GETCW(fpcr) \
   __asm__ __volatile__ ("mrs %0, fpcr" : "=r" (fpcr))
#  define _FPU_SETCW(fpcr) \
   __asm__ __volatile__ ("msr fpcr, %0" : : "r" (fpcr))
# endif

# if __GNUC__ >= 6 && !defined __clang__
#  define _FPU_GETFPSR(fpsr) (fpsr = __builtin_aarch64_get_fpsr ())
#  define _FPU_SETFPSR(fpsr) __builtin_aarch64_set_fpsr (fpsr)
# elif __clang_major__ >= 4
#  define _FPU_GETFPSR(fpsr) (fpsr = __builtin_arm_rsr ("fpsr"))
#  define _FPU_SETFPSR(fpsr) __builtin_arm_wsr ("fpsr", fpsr)
# else
#  define _FPU_GETFPSR(fpsr) \
   __asm__ __volatile__ ("mrs %0, fpsr" : "=r" (fpsr))
#  define _FPU_SETFPSR(fpsr) \
   __asm__ __volatile__ ("msr fpsr, %0" : : "r" (fpsr))
# endif

#elif defined __arm__

/* fpscr bits 23..22 indicate the rounding direction.  */
/* fpscr bits 4..0 indicate which floating-point exceptions have occurred
   since the respective bit was last set to zero.  */
/* fpscr bits 12..8 indicate which floating-point exceptions shall, when
   occurring, trigger a trap rather than merely set the corresponding bit
   in the fpscr register.  */

# if !defined __SOFTFP__
#  define _FPU_GETCW(cw) \
   __asm__ __volatile__ ("vmrs %0, fpscr" : "=r" (cw))
#  define _FPU_SETCW(cw) \
   __asm__ __volatile__ ("vmsr fpscr, %0" : : "r" (cw))
# endif

#elif defined __alpha

/* System calls.  */
extern unsigned long __ieee_get_fp_control (void);
extern void __ieee_set_fp_control (unsigned long);

# define _FPU_GETCW(fpcr) \
  __asm__ __volatile__ ("excb; mf_fpcr %0" : "=f" (fpcr))
# define _FPU_SETCW(fpcr) \
  __asm__ __volatile__ ("mt_fpcr %0; excb" : : "f" (fpcr))

#elif defined __hppa

/* Bits 31..27 of the first 32-bit word of %fr0 indicate which floating-point
   exceptions have occurred since the respective bit was last set to zero.  */
/* Bits 4..0 of the first 32-bit word of %fr0 indicate which floating-point
   exceptions shall, when occurring, trigger a trap rather than merely set the
   corresponding flag bit.  */

/* The status register is located in bits 0 to 31 of floating-point register 0.  */
# define _FPU_GETCW(cw) \
({                                                                              \
  union { __extension__ unsigned long long __fpreg; unsigned int __halfreg[2]; } __fullfp; \
  /* Get the current status word. */                                            \
  __asm__ ("fstd %%fr0,0(%1)\n\t"                                               \
           "fldd 0(%1),%%fr0\n\t"                                               \
           : "=m" (__fullfp.__fpreg) : "r" (&__fullfp.__fpreg) : "%r0");        \
  cw = __fullfp.__halfreg[0];                                                   \
})
# define _FPU_SETCW(cw) \
({                                                                              \
  union { __extension__ unsigned long long __fpreg; unsigned int __halfreg[2]; } __fullfp; \
  /* Get the current status word and set the control word.  */                  \
  __asm__ ("fstd %%fr0,0(%1)\n\t"                                               \
           : "=m" (__fullfp.__fpreg) : "r" (&__fullfp.__fpreg) : "%r0");        \
  __fullfp.__halfreg[0] = cw;                                                   \
  __asm__ ("fldd 0(%1),%%fr0\n\t"                                               \
           : : "m" (__fullfp.__fpreg), "r" (&__fullfp.__fpreg) : "%r0" );       \
})

#elif defined __ia64__

/* fpsr bits 12..9,7 indicate which floating-point exceptions have occurred
   since the respective bit was last set to zero.  */
/* fpsr bits 5..2,0 indicate which floating-point exceptions shall, when
   occurring, *not* trigger a trap rather than merely set the corresponding
   bit in the fpsr register.  */

# define _FPU_GETCW(fpsr) \
  __asm__ __volatile__ ("mov.m %0=ar.fpsr" : "=r" (fpsr))
# define _FPU_SETCW(fpsr) \
  __asm__ __volatile__ ("mov.m ar.fpsr=%0" :: "r" (fpsr) : "memory")

#elif defined __m68k__

/* fpsr bits 7..3 indicate which floating-point exceptions have occurred
   since the respective bit was last set to zero.  */
/* fpcr bits 15..8 indicate which floating-point exceptions shall, when
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
   FE_INVALID corresponds to all three: bit 15, bit 14, bit 13.  */

# define _FPU_GETCW(cw) __asm__ __volatile__ ("fmove%.l %!, %0" : "=dm" (cw))
# define _FPU_SETCW(cw) __asm__ __volatile__ ("fmove%.l %0, %!" : : "dm" (cw))

# define _FPU_GETFPSR(cw) __asm__ __volatile__ ("fmove%.l %/fpsr, %0" : "=dm" (cw))
# define _FPU_SETFPSR(cw) __asm__ __volatile__ ("fmove%.l %0, %/fpsr" : : "dm" (cw))

#elif defined __mips__

/* fcsr bits 6..2 indicate which floating-point exceptions have occurred
   since the respective bit was last set to zero.
   fcsr bits 17..12 indicate which floating-point exceptions have occurred
   in the most recent instruction.  */
/* fcsr bits 11..7 indicate which floating-point exceptions shall, when
   occurring, trigger a trap rather than merely set the corresponding bit
   in the fcsr register.  */

# define _FPU_GETCW(cw) __asm__ __volatile__ ("cfc1 %0,$31" : "=r" (cw))
# define _FPU_SETCW(cw) __asm__ __volatile__ ("ctc1 %0,$31" : : "r" (cw))

#elif defined __loongarch__

/* fcsr0 bits 20..16 indicate which floating-point exceptions have occurred
   since the respective bit was last set to zero.
   fcsr0 bits 28..24 indicate which floating-point exceptions have occurred
   in the most recent instruction.  */
/* fcsr0 bits 4..0 indicate which floating-point exceptions shall, when
   occurring, trigger a trap rather than merely set the corresponding bit
   in the fcsr0 register.  */

# define _FPU_GETCW(cw) __asm__ __volatile__ ("movfcsr2gr %0,$r0" : "=r" (cw))
# define _FPU_SETCW(cw) __asm__ __volatile__ ("movgr2fcsr $r0,%0" : : "r" (cw))

#elif defined _ARCH_PPC

/* fpscr bits 28..25 indicate which floating-point exceptions, other than
   FE_INVALID, have occurred since the respective bit was last set to zero.
   fpscr bits 24..19, 10..8 do the same thing, for various kinds of Invalid
   Operation.  fpscr bit 29 is the summary (the OR) of all these bits.  */
/* fpscr bits 7..3 indicate which floating-point exceptions shall, when
   occurring, trigger a trap rather than merely set the corresponding bit
   in the fpscr register.  */

# define _FPU_GETCW_AS_DOUBLE(cw) \
  do { double env; __asm__ __volatile__ ("mffs %0" : "=f" (env)); cw = env; } \
  while (0)
# define _FPU_SETCW_AS_DOUBLE(cw) \
  __asm__ __volatile__ ("mtfsf 0xff,%0" : : "f" (cw))

# if defined __NetBSD__
/* Modifying the FE0 and FE1 bits of the machine state register (MSR) is
   only possible from the kernel.  NetBSD allows it to be done from user
   space, by emulating the mfmsr and mtmsr instructions when they trap.
   In other words, these instructions are actually system calls in NetBSD.  */
#  define _GETMSR(msr) __asm__ __volatile__ ("mfmsr %0" : "=r" (msr))
#  define _SETMSR(msr) __asm__ __volatile__ ("mtmsr %0" : : "r" (msr))
#  define MSR_FP_EXC_MASK 0x00000900
/* This allows us to simulate the Linux prctl() through a macro.  */
#  define PR_SET_FPEXC 1
#  define PR_FP_EXC_DISABLED  0x00000000  /* FP exceptions disabled */
#  define PR_FP_EXC_NONRECOV  0x00000100  /* async non-recoverable exc. mode */
#  define PR_FP_EXC_ASYNC     0x00000800  /* async recoverable exception mode */
#  define PR_FP_EXC_PRECISE   0x00000900  /* precise exception mode */
#  define prctl(operation,arg) \
     do {                                         \
       if ((operation) == PR_SET_FPEXC)           \
         {                                        \
           unsigned int local_msr;                \
           _GETMSR (local_msr);                   \
           local_msr &= ~MSR_FP_EXC_MASK;         \
           local_msr |= (arg) & MSR_FP_EXC_MASK;  \
           _SETMSR (local_msr);                   \
         }                                        \
     } while (0)
# endif

#elif defined __riscv

/* fcsr bits 4..0 indicate which floating-point exceptions have occurred
   since the respective bit was last set to zero.  */

/* Trapping of floating-point exceptions does not work on RISC-V.  That's
   because the fcsr register has only bits for floating-point exception status,
   but no bits for trapping floating-point exceptions.  */

#elif defined __s390__ || defined __s390x__

/* fpc bits 23..19 indicate which floating-point exceptions have occurred
   since the respective bit was last set to zero.
   fpc bits 15..11 are part of the "data exception code" (DXC) and have a
   similar meaning if bits 9..8 are both zero.  */
/* fpc bits 31..27 indicate which floating-point exceptions shall, when
   occurring, trigger a trap rather than merely set the corresponding bit
   in the fpc register.  */

# define _FPU_GETCW(cw)  __asm__ __volatile__ ("efpc %0" : "=d" (cw))
# define _FPU_SETCW(cw)  __asm__ __volatile__ ("sfpc %0" : : "d" (cw))

#elif defined __sh__

/* fpscr bits 6..2 indicate which floating-point exceptions have occurred
   since the respective bit was last set to zero.  */
/* fpscr bits 11..7 indicate which floating-point exceptions shall, when
   occurring, trigger a trap rather than merely set the corresponding bit
   in the fpscr register.  */

# define _FPU_GETCW(cw) __asm__ ("sts fpscr,%0" : "=r" (cw))
# define _FPU_SETCW(cw) __asm__ ("lds %0,fpscr" : : "r" (cw))

#elif defined __sparc

/* fsr bits 9..5 indicate which floating-point exceptions have occurred
   since the respective bit was last set to zero.  */
/* fsr bits 27..23 indicate which floating-point exceptions shall, when
   occurring, trigger a trap rather than merely set the corresponding bit
   in the fsr register.  */

# if defined __sparcv9 || defined __arch64__ /* sparc64 */
#  define _FPU_GETCW(X)   __asm__ __volatile__ ("stx %%fsr,%0" : "=m" (X))
#  define _FPU_SETCW(X)   __asm__ __volatile__ ("ldx %0,%%fsr" : : "m" (X))
# else
#  define _FPU_GETCW(X)   __asm__ __volatile__ ("st %%fsr,%0" : "=m" (X))
#  define _FPU_SETCW(X)   __asm__ __volatile__ ("ld %0,%%fsr" : : "m" (X))
# endif

#endif

#if defined _AIX && defined _ARCH_PPC /* AIX */

/* <fpxcp.h> defines a type fpflag_t and macros FP_*.  */

/* Convert from an 'int exceptions' to an fpflag_t.  */
# if 0 /* Unoptimized */
#  define exceptions_to_fpflag(exceptions) \
     (  ((exceptions) & FE_INVALID   ? FP_INVALID     : 0) \
      | ((exceptions) & FE_DIVBYZERO ? FP_DIV_BY_ZERO : 0) \
      | ((exceptions) & FE_OVERFLOW  ? FP_OVERFLOW    : 0) \
      | ((exceptions) & FE_UNDERFLOW ? FP_UNDERFLOW   : 0) \
      | ((exceptions) & FE_INEXACT   ? FP_INEXACT     : 0))
# else /* Optimized */
#  define exceptions_to_fpflag(exceptions) \
     ((exceptions) & FE_ALL_EXCEPT)
# endif

/* Convert from an fpflag_t to an 'int exceptions'.  */
# if 0 /* Unoptimized */
#  define fpflag_to_exceptions(f) \
     (  ((f) & FP_INVALID     ? FE_INVALID   : 0) \
      | ((f) & FP_DIV_BY_ZERO ? FE_DIVBYZERO : 0) \
      | ((f) & FP_OVERFLOW    ? FE_OVERFLOW  : 0) \
      | ((f) & FP_UNDERFLOW   ? FE_UNDERFLOW : 0) \
      | ((f) & FP_INEXACT     ? FE_INEXACT   : 0))
# else /* Optimized */
#  define fpflag_to_exceptions(f) \
     ((f) & FE_ALL_EXCEPT)
# endif

/* The implementation of fegetexcept().  Avoids a module dependency.  */
# define fegetexcept_impl() \
    (  (fp_is_enabled (TRP_INVALID)     ? FE_INVALID   : 0) \
     | (fp_is_enabled (TRP_DIV_BY_ZERO) ? FE_DIVBYZERO : 0) \
     | (fp_is_enabled (TRP_OVERFLOW)    ? FE_OVERFLOW  : 0) \
     | (fp_is_enabled (TRP_UNDERFLOW)   ? FE_UNDERFLOW : 0) \
     | (fp_is_enabled (TRP_INEXACT)     ? FE_INEXACT   : 0))

#endif
