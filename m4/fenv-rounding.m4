# fenv-rounding.m4
# serial 3
dnl Copyright (C) 2023-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN([gl_FENV_ROUNDING],
[
  AC_REQUIRE([gl_FENV_H_DEFAULTS])
  AC_REQUIRE([AC_CANONICAL_HOST])

  dnl On FreeBSD 11/i386, fesetround needs -lm while fegetround doesn't.
  gl_MATHFUNC([fesetround], [int], [(int)], [#include <fenv.h>])
  if test $gl_cv_func_fesetround_no_libm = yes \
     || test $gl_cv_func_fesetround_in_libm = yes; then
    dnl The fesetround function does not work on MSVC 14.
    AC_CACHE_CHECK([whether fesetround works],
      [gl_cv_func_fesetround_works],
      [case "$host_os" in
         # Guess no on native Windows other than mingw.
         mingw* | windows*)
           AC_EGREP_CPP([Problem], [
#ifndef __MINGW32__
 Problem
#endif
             ],
             [gl_cv_func_fesetround_works="guessing no"],
             [gl_cv_func_fesetround_works="guessing yes"])
           ;;
         *) gl_cv_func_fesetround_works="guessing yes" ;;
       esac
      ])
    case "$gl_cv_func_fesetround_works" in
      *yes) ;;
      *) REPLACE_FESETROUND=1 ;;
    esac
    if test $REPLACE_FESETROUND = 1; then
      dnl One more function is defined in the same compilation unit.
      REPLACE_FEGETROUND=1
      FENV_ROUNDING_LIBM=
    else
      dnl It needs linking with -lm on
      dnl glibc, FreeBSD, NetBSD, OpenBSD, AIX, HP-UX, Solaris, Android.
      if test $gl_cv_func_fesetround_no_libm = yes; then
        FENV_ROUNDING_LIBM=
      else
        FENV_ROUNDING_LIBM=-lm
      fi
    fi
  else
    HAVE_FEGETROUND=0
    HAVE_FESETROUND=0
    FENV_ROUNDING_LIBM=
  fi
  if test $HAVE_FESETROUND = 0 || test $REPLACE_FESETROUND = 1; then
    gl_PREREQ_FENV_ROUNDING
    dnl Possibly need -lm for fpgetround(), fpsetround().
    if test $gl_cv_func_fpsetround_no_libm = no \
       && test $gl_cv_func_fpsetround_in_libm = yes \
       && test -z "$FENV_ROUNDING_LIBM"; then
      FENV_ROUNDING_LIBM=-lm
    fi
  fi
  AC_SUBST([FENV_ROUNDING_LIBM])
])

dnl Prerequisites of lib/fenv-round.c.
AC_DEFUN([gl_PREREQ_FENV_ROUNDING],
[
  gl_MATHFUNC([fpsetround], [fp_rnd], [(fp_rnd)],
    [#include <ieeefp.h>
    ])
  if test $gl_cv_func_fpsetround_no_libm = yes \
     || test $gl_cv_func_fpsetround_in_libm = yes; then
    AC_DEFINE([HAVE_FPSETROUND], [1],
      [Define to 1 if you have the 'fpsetround' function.])
  fi
])
