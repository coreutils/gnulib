/* A GNU-like <fenv.h>.

   Copyright (C) 1997-2023 Free Software Foundation, Inc.

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

/* Based on glibc/sysdeps/<cpu>/bits/fenv.h.  */

#if __GNUC__ >= 3
@PRAGMA_SYSTEM_HEADER@
#endif
@PRAGMA_COLUMNS@

#ifndef _@GUARD_PREFIX@_FENV_H

/* The include_next requires a split double-inclusion guard.  */
#if @HAVE_FENV_H@
# @INCLUDE_NEXT@ @NEXT_FENV_H@
#endif

#ifndef _@GUARD_PREFIX@_FENV_H
#define _@GUARD_PREFIX@_FENV_H

/* This file uses GNULIB_POSIXCHECK.  */
#if !_GL_CONFIG_H_INCLUDED
 #error "Please include config.h first."
#endif

/* The definitions of _GL_FUNCDECL_RPL etc. are copied here.  */

/* The definition of _GL_WARN_ON_USE is copied here.  */


/* We cannot implement the #pragmas here; this needs to be done in the
   compiler.  */


/* ISO C 99 § 7.6.4 Environment
   ISO C 23 § 7.6.6 Environment  */

#if !@HAVE_FENV_H@

# if (defined __x86_64__ || defined _M_X64) || (defined __i386 || defined _M_IX86)

typedef struct
  {
    /* 7 32-bit words, see <https://stackoverflow.com/questions/10337750/>  */
    unsigned short __control_word;         /* fctrl register */
    unsigned short __reserved1;
    unsigned short __status_word;          /* fstat register */
    unsigned short __reserved2;
    unsigned short __tags;
    unsigned short __reserved3;
    unsigned int __instruction_pointer;    /* eip register */
    unsigned short __instruction_selector; /* cs register */
    unsigned int __opcode : 11;
    unsigned int __reserved4 : 5;
    unsigned int __data_pointer;
    unsigned short __data_selector;
    unsigned short __reserved5;
#  ifdef __x86_64__
    unsigned int __mxcsr;                  /* mxcsr register */
#  endif
  }
fenv_t;

# elif defined __aarch64__ /* arm64 */

typedef struct
  {
    unsigned int __fpcr; /* fpcr register */
    unsigned int __fpsr; /* fpsr register */
  }
fenv_t;

# elif defined __arm__

typedef struct
  {
    unsigned int __cw; /* fpscr register */
  }
fenv_t;

# elif defined __alpha

typedef unsigned long fenv_t;

# elif defined __hppa

typedef struct
{
  unsigned int __status_word;  /* floating point status register */
  unsigned int __exception[7]; /* floating point exception registers */
} fenv_t;

# elif defined __ia64__

typedef unsigned long fenv_t; /* fpsr = floating point status register */

# elif defined __m68k__

typedef struct
  {
    unsigned int __control_register;    /* fpcr = floating point control register */
    unsigned int __status_register;     /* fpsr = floating point status register */
    unsigned int __instruction_address; /* floating point instruction register */
  }
fenv_t;

# elif defined __mips__ || defined __loongarch__

typedef struct
  {
    unsigned int __fp_control_register; /* floating point control and status register */
  }
fenv_t;

# elif defined __powerpc__

typedef double /* yuck! */ fenv_t; /* fpscr register */

# elif defined __riscv

typedef unsigned int fenv_t; /* fcsr register */

# elif defined __s390__ || defined __s390x__

typedef struct
  {
    unsigned int __fpc; /* fpc = floating point control register */
  }
fenv_t;

# elif defined __sh__

typedef struct
  {
    unsigned int __fpscr; /* fpscr register */
  }
fenv_t;

# elif defined __sparc

typedef unsigned long fenv_t; /* fsr = floating point state register */

#else

/* A dummy fallback.  */

typedef unsigned long fenv_t;

# endif

# define FE_DFL_ENV ((const fenv_t *) (-1))

#endif


/* ISO C 99 § 7.6.3 Rounding
   ISO C 23 § 7.6.5 Rounding  */

#if !@HAVE_FENV_H@

# if (defined __x86_64__ || defined _M_X64) || (defined __i386 || defined _M_IX86)

/* Attention: MSVC has different values for these macros!  */
#  define FE_TONEAREST   (0 << 10)
#  define FE_DOWNWARD    (1 << 10)
#  define FE_UPWARD      (2 << 10)
#  define FE_TOWARDZERO  (3 << 10)

# elif defined __aarch64__ /* arm64 */

/* Attention: FreeBSD libc has these values shifted right by 22 bits!  */
#  define FE_TONEAREST   (0 << 22)
#  define FE_UPWARD      (1 << 22)
#  define FE_DOWNWARD    (2 << 22)
#  define FE_TOWARDZERO  (3 << 22)

# elif defined __arm__

#  define FE_TONEAREST   (0 << 22)
#  if !defined __SOFTFP__
#   define FE_UPWARD      (1 << 22)
#   define FE_DOWNWARD    (2 << 22)
#   define FE_TOWARDZERO  (3 << 22)
#  endif

# elif defined __alpha

#  define FE_TOWARDZERO  0
#  define FE_DOWNWARD    1
#  define FE_TONEAREST   2
#  define FE_UPWARD      3

# elif defined __hppa

#  define FE_TONEAREST   (0 << 9)
#  define FE_TOWARDZERO  (1 << 9)
#  define FE_UPWARD      (2 << 9)
#  define FE_DOWNWARD    (3 << 9)

# elif defined __ia64__

#  define FE_TONEAREST   0
#  define FE_DOWNWARD    1
#  define FE_UPWARD      2
#  define FE_TOWARDZERO  3

# elif defined __m68k__

#  define FE_TONEAREST   (0 << 4)
#  define FE_TOWARDZERO  (1 << 4)
#  define FE_DOWNWARD    (2 << 4)
#  define FE_UPWARD      (3 << 4)

# elif defined __mips__

#  define FE_TONEAREST   0
#  define FE_TOWARDZERO  1
#  define FE_UPWARD      2
#  define FE_DOWNWARD    3

# elif defined __loongarch__

#  define FE_TONEAREST   (0 << 8)
#  define FE_TOWARDZERO  (1 << 8)
#  define FE_UPWARD      (2 << 8)
#  define FE_DOWNWARD    (3 << 8)

# elif defined __powerpc__

/* Attention: AIX has different values for these macros!  */
#  define FE_TONEAREST   0
#  define FE_TOWARDZERO  1
#  define FE_UPWARD      2
#  define FE_DOWNWARD    3

# elif defined __riscv

/* Attention: FreeBSD libc has these values shifted left by 5 bits!  */
#  define FE_TONEAREST   0
#  define FE_TOWARDZERO  1
#  define FE_DOWNWARD    2
#  define FE_UPWARD      3
#  if 0 /* non-standard */
#   define FE_TONEARESTMAXMAGNITUDE 4
#  endif

# elif defined __s390__ || defined __s390x__

#  define FE_TONEAREST   0
#  define FE_TOWARDZERO  1
#  define FE_UPWARD      2
#  define FE_DOWNWARD    3

# elif defined __sh__

#  define FE_TONEAREST   0
#  define FE_TOWARDZERO  1

# elif defined __sparc

/* Attention: FreeBSD libc has these values shifted right by 30 bits!  */
#  define FE_TONEAREST   (0U << 30)
#  define FE_TOWARDZERO  (1U << 30)
#  define FE_UPWARD      (2U << 30)
#  define FE_DOWNWARD    (3U << 30)

#else

/* A dummy fallback.  */

#  define FE_TONEAREST   0

# endif

#endif

#if @GNULIB_FEGETROUND@
/* Returns the current rounding direction.  */
# if @REPLACE_FEGETROUND@ || (!@HAVE_FEGETROUND@ && (defined __GLIBC__ || defined __FreeBSD__)) /* has an inline definition */
#  if !(defined __cplusplus && defined GNULIB_NAMESPACE)
#   undef fegetround
#   define fegetround rpl_fegetround
#  endif
_GL_FUNCDECL_RPL (fegetround, int, (void));
_GL_CXXALIAS_RPL (fegetround, int, (void));
# else
#  if !@HAVE_FEGETROUND@
_GL_FUNCDECL_SYS (fegetround, int, (void));
#  endif
_GL_CXXALIAS_SYS (fegetround, int, (void));
# endif
_GL_CXXALIASWARN (fegetround);
#endif

#if @GNULIB_FESETROUND@
/* Sets the rounding direction of the current thread.
   Returns zero if the argument is valid and the operation was thus successful.
   Returns non-zero upon failure.  */
# if @REPLACE_FESETROUND@ || (!@HAVE_FESETROUND@ && defined __FreeBSD__) /* has an inline definition */
#  if !(defined __cplusplus && defined GNULIB_NAMESPACE)
#   undef fesetround
#   define fesetround rpl_fesetround
#  endif
_GL_FUNCDECL_RPL (fesetround, int, (int rounding_direction));
_GL_CXXALIAS_RPL (fesetround, int, (int rounding_direction));
# else
#  if !@HAVE_FESETROUND@
_GL_FUNCDECL_SYS (fesetround, int, (int rounding_direction));
#  endif
_GL_CXXALIAS_SYS (fesetround, int, (int rounding_direction));
# endif
_GL_CXXALIASWARN (fesetround);
#endif


/* ISO C 99 § 7.6.2 Floating-point exceptions
   ISO C 23 § 7.6.4 Floating-point exceptions
   API without fexcept_t  */

#if !@HAVE_FENV_H@

# if (defined __x86_64__ || defined _M_X64) || (defined __i386 || defined _M_IX86)

/* Attention: MSVC has different values for these macros!  */
#  define FE_INVALID    (1 << 0)
#  if 0 /* non-standard */
#   define FE_DENORMAL  (1 << 1)
#  endif
#  define FE_DIVBYZERO  (1 << 2)
#  define FE_OVERFLOW   (1 << 3)
#  define FE_UNDERFLOW  (1 << 4)
#  define FE_INEXACT    (1 << 5)

# elif defined __aarch64__ /* arm64 */ || defined __arm__

#  define FE_INVALID    (1U << 0)
#  define FE_DIVBYZERO  (1U << 1)
#  define FE_OVERFLOW   (1U << 2)
#  define FE_UNDERFLOW  (1U << 3)
#  define FE_INEXACT    (1U << 4)

# elif defined __alpha

/* Attention: The *BSDs have these values shifted right by 17 bits!  */
#  define FE_INVALID    (1UL << 17)
#  define FE_DIVBYZERO  (1UL << 18)
#  define FE_OVERFLOW   (1UL << 19)
#  define FE_UNDERFLOW  (1UL << 20)
#  define FE_INEXACT    (1UL << 21)
#  if 0 /* non-standard */
#   define FE_DENORMAL  (1UL << 22)
#  endif

# elif defined __hppa

#  define FE_INEXACT    (1U << 0)
#  define FE_UNDERFLOW  (1U << 1)
#  define FE_OVERFLOW   (1U << 2)
#  define FE_DIVBYZERO  (1U << 3)
#  define FE_INVALID    (1U << 4)

# elif defined __ia64__

#  define FE_INVALID    (1UL << 0)
#  if 0 /* non-standard */
#   define FE_UNNORMAL  (1UL << 1)
#  endif
#  define FE_DIVBYZERO  (1UL << 2)
#  define FE_OVERFLOW   (1UL << 3)
#  define FE_UNDERFLOW  (1UL << 4)
#  define FE_INEXACT    (1UL << 5)

# elif defined __m68k__

#  define FE_INEXACT    (1U << 3)
#  define FE_DIVBYZERO  (1U << 4)
#  define FE_UNDERFLOW  (1U << 5)
#  define FE_OVERFLOW   (1U << 6)
#  define FE_INVALID    (1U << 7)

# elif defined __mips__

#  define FE_INEXACT    (1 << 2)
#  define FE_UNDERFLOW  (1 << 3)
#  define FE_OVERFLOW   (1 << 4)
#  define FE_DIVBYZERO  (1 << 5)
#  define FE_INVALID    (1 << 6)

# elif defined __loongarch__

#  define FE_INEXACT    (1U << 16)
#  define FE_UNDERFLOW  (1U << 17)
#  define FE_OVERFLOW   (1U << 18)
#  define FE_DIVBYZERO  (1U << 19)
#  define FE_INVALID    (1U << 20)

# elif defined __powerpc__

#  define FE_INEXACT    (1U << 25)
#  define FE_DIVBYZERO  (1U << 26)
#  define FE_UNDERFLOW  (1U << 27)
#  define FE_OVERFLOW   (1U << 28)
#  define FE_INVALID    (1U << 29)

# elif defined __riscv

#  define FE_INEXACT    (1U << 0)
#  define FE_UNDERFLOW  (1U << 1)
#  define FE_OVERFLOW   (1U << 2)
#  define FE_DIVBYZERO  (1U << 3)
#  define FE_INVALID    (1U << 4)

# elif defined __s390__ || defined __s390x__

/* Attention: musl libc has these values shifted left by 16 bits!  */
#  define FE_INEXACT    (1U << 3)
#  define FE_UNDERFLOW  (1U << 4)
#  define FE_OVERFLOW   (1U << 5)
#  define FE_DIVBYZERO  (1U << 6)
#  define FE_INVALID    (1U << 7)

# elif defined __sh__

#  define FE_INEXACT    (1 << 2)
#  define FE_UNDERFLOW  (1 << 3)
#  define FE_OVERFLOW   (1 << 4)
#  define FE_DIVBYZERO  (1 << 5)
#  define FE_INVALID    (1 << 6)

# elif defined __sparc

/* Attention: Solaris has these values shifted right by 5 bits!  */
#  define FE_INEXACT    (1UL << 5)
#  define FE_DIVBYZERO  (1UL << 6)
#  define FE_UNDERFLOW  (1UL << 7)
#  define FE_OVERFLOW   (1UL << 8)
#  define FE_INVALID    (1UL << 9)

# endif

# define FE_ALL_EXCEPT \
   (FE_DIVBYZERO | FE_INEXACT | FE_INVALID | FE_OVERFLOW | FE_UNDERFLOW)

#endif

#if @GNULIB_FECLEAREXCEPT@
/* Clears the specified exception flags, and returns 0.
   Upon failure, it returns non-zero.  */
# if @REPLACE_FECLEAREXCEPT@ || (!@HAVE_FECLEAREXCEPT@ && (defined __GLIBC__ || defined __FreeBSD__)) /* has an inline definition */
#  if !(defined __cplusplus && defined GNULIB_NAMESPACE)
#   undef feclearexcept
#   define feclearexcept rpl_feclearexcept
#  endif
_GL_FUNCDECL_RPL (feclearexcept, int, (int exceptions));
_GL_CXXALIAS_RPL (feclearexcept, int, (int exceptions));
# else
#  if !@HAVE_FECLEAREXCEPT@
_GL_FUNCDECL_SYS (feclearexcept, int, (int exceptions));
#  endif
_GL_CXXALIAS_SYS (feclearexcept, int, (int exceptions));
# endif
_GL_CXXALIASWARN (feclearexcept);
#endif

#if @GNULIB_FERAISEEXCEPT@
/* Sets the specified exception flags, triggering handlers or traps if enabled,
   and returns 0.  Upon failure, it returns non-zero.  */
# if @REPLACE_FERAISEEXCEPT@ || (!@HAVE_FERAISEEXCEPT@ && (defined __GLIBC__ || defined __FreeBSD__ || defined _MSC_VER)) /* has an inline definition */
#  if !(defined __cplusplus && defined GNULIB_NAMESPACE)
#   undef feraiseexcept
#   define feraiseexcept rpl_feraiseexcept
#  endif
_GL_FUNCDECL_RPL (feraiseexcept, int, (int exceptions));
_GL_CXXALIAS_RPL (feraiseexcept, int, (int exceptions));
# else
#  if !@HAVE_FERAISEEXCEPT@
_GL_FUNCDECL_SYS (feraiseexcept, int, (int exceptions));
#  endif
_GL_CXXALIAS_SYS (feraiseexcept, int, (int exceptions));
# endif
_GL_CXXALIASWARN (feraiseexcept);
#endif

#if @GNULIB_FETESTEXCEPT@
/* Returns a bitmask of those exception flags among EXCEPTIONS that are
   currently set.  */
# if @REPLACE_FETESTEXCEPT@ || (!@HAVE_FETESTEXCEPT@ && defined __FreeBSD__) /* has an inline definition */
#  if !(defined __cplusplus && defined GNULIB_NAMESPACE)
#   undef fetestexcept
#   define fetestexcept rpl_fetestexcept
#  endif
_GL_FUNCDECL_RPL (fetestexcept, int, (int exceptions));
_GL_CXXALIAS_RPL (fetestexcept, int, (int exceptions));
# else
#  if !@HAVE_FETESTEXCEPT@
_GL_FUNCDECL_SYS (fetestexcept, int, (int exceptions));
#  endif
_GL_CXXALIAS_SYS (fetestexcept, int, (int exceptions));
# endif
_GL_CXXALIASWARN (fetestexcept);
#endif


/* ISO C 99 § 7.6.2 Floating-point exceptions
   ISO C 23 § 7.6.4 Floating-point exceptions
   API with fexcept_t  */

#if !@HAVE_FENV_H@

# if (defined __x86_64__ || defined _M_X64) || (defined __i386 || defined _M_IX86)

typedef unsigned short fexcept_t;

# elif defined __aarch64__ /* arm64 */ || defined __arm__

typedef unsigned int fexcept_t;

# elif defined __alpha

typedef unsigned long fexcept_t;

# elif defined __hppa

typedef unsigned int fexcept_t;

# elif defined __ia64__

typedef unsigned long fexcept_t;

# elif defined __m68k__

typedef unsigned int fexcept_t;

# elif defined __mips__

typedef unsigned short fexcept_t;

# elif defined __loongarch__

typedef unsigned int fexcept_t;

# elif defined __powerpc__

typedef unsigned int fexcept_t;

# elif defined __riscv

typedef unsigned int fexcept_t;

# elif defined __s390__ || defined __s390x__

typedef unsigned int fexcept_t;

# elif defined __sh__

typedef unsigned short fexcept_t;

# elif defined __sparc

typedef unsigned long fexcept_t;

# else

/* A dummy fallback.  */

typedef unsigned long fexcept_t;

# endif

#endif


#endif /* _@GUARD_PREFIX@_FENV_H */
#endif /* _@GUARD_PREFIX@_FENV_H */
