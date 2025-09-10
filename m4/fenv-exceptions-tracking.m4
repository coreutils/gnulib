# fenv-exceptions-tracking.m4
# serial 5
dnl Copyright (C) 2023-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN_ONCE([gl_FENV_EXCEPTIONS_TRACKING],
[
  AC_REQUIRE([gl_FENV_H_DEFAULTS])
  AC_REQUIRE([AC_CANONICAL_HOST])

  gl_MATHFUNC([feraiseexcept], [int], [(int)], [#include <fenv.h>])
  if test $gl_cv_func_feraiseexcept_no_libm = yes \
     || test $gl_cv_func_feraiseexcept_in_libm = yes; then
    dnl On glibc 2.19/arm, feraiseexcept does not detect failures.
    dnl Fixed through
    dnl <https://sourceware.org/git/?p=glibc.git;a=commitdiff;h=1a2f40e5d14ed6450696feacf04fca5eeceae7ef>.
    dnl On Cygwin 3.4.6/x86_64, feraiseexcept does not trigger traps.
    dnl See <https://sourceware.org/pipermail/cygwin/2023-October/254667.html>.
    dnl On musl libc, on those CPUs where fenv-except-tracking-raise.c
    dnl uses the "generic" approach, feraiseexcept does not trigger traps
    dnl because it merely manipulates flags in the control register.
    dnl On NetBSD 9.3/x86_64, fetestexcept clears the exception trap bits,
    dnl causing the fenv-environment tests to fail.
    case "$host" in
      arm*-*-linux*)
        AC_CACHE_CHECK([whether feraiseexcept works],
          [gl_cv_func_feraiseexcept_works],
          [AC_COMPILE_IFELSE(
             [AC_LANG_PROGRAM([[
                #ifdef __SOFTFP__
                 #include <string.h> /* for __GNU_LIBRARY__ */
                 #ifdef __GNU_LIBRARY__
                  #include <features.h>
                  #if __GLIBC__ == 2 && __GLIBC_MINOR__ <= 19
                   Unlucky user
                  #endif
                 #endif
                #endif
                ]],
                [])],
             [gl_cv_func_feraiseexcept_works="guessing yes"],
             [gl_cv_func_feraiseexcept_works="guessing no"])
          ])
        case "$gl_cv_func_feraiseexcept_works" in
          *yes) ;;
          *) REPLACE_FERAISEEXCEPT=1 ;;
        esac
        ;;
      x86_64-*-cygwin*)
        AC_CACHE_CHECK([whether feraiseexcept works],
          [gl_cv_func_feraiseexcept_works],
          [AC_RUN_IFELSE(
             [AC_LANG_PROGRAM([[
                #include <fenv.h>
                ]],
                [[feclearexcept (FE_INVALID);
                  feenableexcept (FE_INVALID);
                  feraiseexcept (FE_INVALID);
                  return 0;
                ]])
             ],
             [gl_cv_func_feraiseexcept_works=no],
             [gl_cv_func_feraiseexcept_works=yes],
             [gl_cv_func_feraiseexcept_works="guessing no"])
          ])
        case "$gl_cv_func_feraiseexcept_works" in
          *yes) ;;
          *) REPLACE_FERAISEEXCEPT=1 ;;
        esac
        ;;
      *-*-*-musl* | *-*-midipix*)
        dnl This is only needed when the module 'fenv-exceptions-trapping' is
        dnl in use.
        m4_ifdef([g][l_FENV_EXCEPTIONS_TRAPPING], [
          AC_CACHE_CHECK([whether feraiseexcept works],
            [gl_cv_func_feraiseexcept_works],
            [case "$host_cpu" in
               aarch64* | loongarch* | m68k* | s390* | sh4* | *86* | sparc*)
                 gl_cv_func_feraiseexcept_works="guessing no" ;;
               *)
                 gl_cv_func_feraiseexcept_works="guessing yes" ;;
             esac
            ])
          case "$gl_cv_func_feraiseexcept_works" in
            *yes) ;;
            *) REPLACE_FERAISEEXCEPT=1 ;;
          esac
        ])
        ;;
      powerpc*-*-aix*)
        dnl On AIX, the unit test test-fenv-except-state-1 fails if we don't
        dnl override feraiseexcept. XXX Not reproducible any more.
        dnl REPLACE_FERAISEEXCEPT=1
        ;;
      *86*-*-mingw* | *86*-*-windows*)
        dnl On MSVC 14/clang, without this override, there are test failures
        dnl (but not with MSVC 14 itself). Maybe fetestexcept does not consider
        dnl the exception flags in the SSE unit? It's not clear.
        AC_CACHE_CHECK([whether fetestexcept works],
          [gl_cv_func_fetestexcept_works],
          [AC_EGREP_CPP([Problem], [
#ifndef __MINGW32__
 Problem
#endif
             ],
             [gl_cv_func_fetestexcept_works="guessing no"],
             [gl_cv_func_fetestexcept_works="guessing yes"])
          ])
        case "$gl_cv_func_fetestexcept_works" in
          *yes) ;;
          *) REPLACE_FERAISEEXCEPT=1 ;;
        esac
        ;;
      x86_64-*-netbsd*)
        dnl On NetBSD 9.3/x86_64, fetestexcept is broken: it clears the
        dnl floating-point exception trap bits (because it uses an
        dnl 'fnstenv' instruction without subsequent 'fldenv' or 'fldcw').
        REPLACE_FETESTEXCEPT=1
        ;;
      mips*-*-openbsd*)
        dnl On OpenBSD 7.4/mips64, the feclearexcept function does not work:
        dnl it misses to clear the "cause bits".
        REPLACE_FECLEAREXCEPT=1
        ;;
      *86*-*-minix*)
        dnl On Minix 3.3, both the feclearexcept and fetestexcept functions
        dnl need to be overridden; otherwise we get unit test failures.
        REPLACE_FECLEAREXCEPT=1
        REPLACE_FETESTEXCEPT=1
        ;;
    esac
    if test $REPLACE_FECLEAREXCEPT = 1 \
       && test $REPLACE_FETESTEXCEPT = 1 \
       && test $REPLACE_FERAISEEXCEPT = 1; then
      FENV_EXCEPTIONS_TRACKING_LIBM=
    else
      dnl It needs linking with -lm on
      dnl glibc, FreeBSD, NetBSD, OpenBSD, AIX, HP-UX, Solaris, Android.
      if test $gl_cv_func_feraiseexcept_no_libm = yes; then
        FENV_EXCEPTIONS_TRACKING_LIBM=
      else
        FENV_EXCEPTIONS_TRACKING_LIBM=-lm
      fi
    fi
  else
    HAVE_FECLEAREXCEPT=0
    HAVE_FETESTEXCEPT=0
    HAVE_FERAISEEXCEPT=0
    case "$host" in
      alpha*-*-linux*)
        dnl For feraiseexcept, which we take from libm.
        FENV_EXCEPTIONS_TRACKING_LIBM=-lm
        ;;
      *)
        FENV_EXCEPTIONS_TRACKING_LIBM=
        ;;
    esac
  fi
  if { test $HAVE_FECLEAREXCEPT = 0 || test $REPLACE_FECLEAREXCEPT = 1; } \
     || { test $HAVE_FETESTEXCEPT = 0 || test $REPLACE_FETESTEXCEPT = 1; }; then
    gl_PREREQ_FENV_EXCEPTIONS
    dnl Possibly need -lm for fpgetsticky(), fpsetsticky().
    if test $gl_cv_func_fpsetsticky_no_libm = no \
       && test $gl_cv_func_fpsetsticky_in_libm = yes \
       && test -z "$FENV_EXCEPTIONS_TRACKING_LIBM"; then
      FENV_EXCEPTIONS_TRACKING_LIBM=-lm
    fi
  fi
  AC_SUBST([FENV_EXCEPTIONS_TRACKING_LIBM])
])
