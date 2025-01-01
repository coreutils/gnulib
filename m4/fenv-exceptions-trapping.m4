# fenv-exceptions-trapping.m4
# serial 3
dnl Copyright (C) 2023-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN_ONCE([gl_FENV_EXCEPTIONS_TRAPPING],
[
  AC_REQUIRE([gl_FENV_H_DEFAULTS])
  dnl Persuade glibc <fenv.h> to declare feenableexcept().
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])
  AC_REQUIRE([AC_CANONICAL_HOST])

  gl_MATHFUNC([feenableexcept], [int], [(int)], [#include <fenv.h>])
  if test $gl_cv_func_feenableexcept_no_libm = yes \
     || test $gl_cv_func_feenableexcept_in_libm = yes; then
    dnl On glibc 2.19/aarch64, feenableexcept does not detect failures.
    dnl Fixed through
    dnl <https://sourceware.org/git/?p=glibc.git;a=commitdiff;h=302949e2940a9da3f6364a1574619e621b7e1e71>.
    dnl Similarly on FreeBSD 12.2/arm, FreeBSD 12.2/arm64, NetBSD 10.0/arm64.
    case "$host" in
      aarch64*-*-linux*)
        AC_CACHE_CHECK([whether feenableexcept works],
          [gl_cv_func_feenableexcept_works],
          [AC_COMPILE_IFELSE(
             [AC_LANG_PROGRAM([[
                #include <string.h> /* for __GNU_LIBRARY__ */
                #ifdef __GNU_LIBRARY__
                 #include <features.h>
                 #if __GLIBC__ == 2 && __GLIBC_MINOR__ <= 19
                  Unlucky user
                 #endif
                #endif
                ]],
                [])],
             [gl_cv_func_feenableexcept_works="guessing yes"],
             [gl_cv_func_feenableexcept_works="guessing no"])
          ])
        case "$gl_cv_func_feenableexcept_works" in
          *yes) ;;
          *) REPLACE_FEENABLEEXCEPT=1 ;;
        esac
        ;;
      arm*-*-freebsd* | aarch64*-*-freebsd* | aarch64*-*-netbsd*)
        REPLACE_FEENABLEEXCEPT=1
        ;;
    esac
    if test $REPLACE_FEENABLEEXCEPT = 1; then
      dnl Two more functions are defined in the same compilation unit.
      dnl Override them as well.
      REPLACE_FEDISABLEEXCEPT=1
      REPLACE_FEGETEXCEPT=1
      FENV_EXCEPTIONS_TRAPPING_LIBM=
    else
      dnl It needs linking with -lm on glibc.
      if test $gl_cv_func_feenableexcept_no_libm = yes; then
        FENV_EXCEPTIONS_TRAPPING_LIBM=
      else
        FENV_EXCEPTIONS_TRAPPING_LIBM=-lm
      fi
    fi
  else
    HAVE_FEENABLEEXCEPT=0
    HAVE_FEDISABLEEXCEPT=0
    HAVE_FEGETEXCEPT=0
    case "$host" in
      alpha*-*-linux*)
        dnl For feenableexcept, which we take from libm.
        FENV_EXCEPTIONS_TRAPPING_LIBM=-lm
        ;;
      *)
        FENV_EXCEPTIONS_TRAPPING_LIBM=
        ;;
    esac
  fi
  if test $HAVE_FEENABLEEXCEPT = 0 || test $REPLACE_FEENABLEEXCEPT = 1; then
    gl_PREREQ_FENV_EXCEPTIONS_TRAPPING
    dnl Possibly need -lm for fpgetmask(), fpsetmask().
    if test $gl_cv_func_fpsetmask_no_libm = no \
       && test $gl_cv_func_fpsetmask_in_libm = yes \
       && test -z "$FENV_EXCEPTIONS_TRAPPING_LIBM"; then
      FENV_EXCEPTIONS_TRAPPING_LIBM=-lm
    fi
    dnl Possibly need -lm for fegettrapenable(), fesettrapenable().
    if test $gl_cv_func_fesettrapenable_no_libm = no \
       && test $gl_cv_func_fesettrapenable_in_libm = yes \
       && test -z "$FENV_EXCEPTIONS_TRAPPING_LIBM"; then
      FENV_EXCEPTIONS_TRAPPING_LIBM=-lm
    fi
  fi
  AC_SUBST([FENV_EXCEPTIONS_TRAPPING_LIBM])
])

dnl Prerequisites of lib/fenv-except-trapping.c.
AC_DEFUN([gl_PREREQ_FENV_EXCEPTIONS_TRAPPING],
[
  gl_MATHFUNC([fpsetmask], [fp_except_t], [(fp_except_t)],
    [#include <ieeefp.h>
     /* The type is called 'fp_except_t' on FreeBSD, but 'fp_except' on
        all other systems.  */
     #if !defined __FreeBSD__
     #define fp_except_t fp_except
     #endif
    ])
  if test $gl_cv_func_fpsetmask_no_libm = yes \
     || test $gl_cv_func_fpsetmask_in_libm = yes; then
    AC_DEFINE([HAVE_FPSETMASK], [1],
      [Define to 1 if you have the 'fpsetmask' function.])
  fi

  gl_MATHFUNC([fesettrapenable], [int], [(int)], [#include <fenv.h>])
  if test $gl_cv_func_fesettrapenable_no_libm = yes \
     || test $gl_cv_func_fesettrapenable_in_libm = yes; then
    AC_DEFINE([HAVE_FESETTRAPENABLE], [1],
      [Define to 1 if you have the 'fesettrapenable' function.])
  fi
])
