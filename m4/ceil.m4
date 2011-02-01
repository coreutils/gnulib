# ceil.m4 serial 5
dnl Copyright (C) 2007, 2009-2011 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_CEIL],
[
  m4_divert_text([DEFAULTS], [gl_ceil_required=plain])
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  dnl Test whether ceil() can be used without libm.
  gl_FUNC_CEIL_LIBS
  if test "$CEIL_LIBM" = "?"; then
    CEIL_LIBM=
  fi
  m4_ifdef([gl_FUNC_CEIL_IEEE], [
    if test $gl_ceil_required = ieee && test $REPLACE_CEIL = 0; then
      AC_CACHE_CHECK([whether ceil works according to ISO C 99 with IEC 60559],
        [gl_cv_func_ceil_ieee],
        [
          save_LIBS="$LIBS"
          LIBS="$LIBS $CEIL_LIBM"
          AC_RUN_IFELSE(
            [AC_LANG_SOURCE([[
#ifndef __NO_MATH_INLINES
# define __NO_MATH_INLINES 1 /* for glibc */
#endif
#include <math.h>
]gl_DOUBLE_MINUS_ZERO_CODE[
]gl_DOUBLE_SIGNBIT_CODE[
int main()
{
  /* Test whether ceil (-0.0) is -0.0.  */
  if (signbitd (minus_zerod) && !signbitd (ceil (minus_zerod)))
    return 1;
  return 0;
}
            ]])],
            [gl_cv_func_ceil_ieee=yes],
            [gl_cv_func_ceil_ieee=no],
            [gl_cv_func_ceil_ieee="guessing no"])
          LIBS="$save_LIBS"
        ])
      case "$gl_cv_func_ceil_ieee" in
        *yes) ;;
        *) REPLACE_CEIL=1 ;;
      esac
    fi
  ])
  if test $REPLACE_CEIL = 1; then
    AC_LIBOBJ([ceil])
    CEIL_LIBM=
  fi
  AC_SUBST([CEIL_LIBM])
])

# Determines the libraries needed to get the ceil() function.
# Sets CEIL_LIBM.
AC_DEFUN([gl_FUNC_CEIL_LIBS],
[
  gl_CACHE_VAL_SILENT([gl_cv_func_ceil_libm], [
    gl_cv_func_ceil_libm=?
    AC_LINK_IFELSE(
      [AC_LANG_PROGRAM(
         [[#ifndef __NO_MATH_INLINES
           # define __NO_MATH_INLINES 1 /* for glibc */
           #endif
           #include <math.h>
           double x;]],
         [[x = ceil(x);]])],
      [gl_cv_func_ceil_libm=])
    if test "$gl_cv_func_ceil_libm" = "?"; then
      save_LIBS="$LIBS"
      LIBS="$LIBS -lm"
      AC_LINK_IFELSE(
        [AC_LANG_PROGRAM(
           [[#ifndef __NO_MATH_INLINES
             # define __NO_MATH_INLINES 1 /* for glibc */
             #endif
             #include <math.h>
             double x;]],
           [[x = ceil(x);]])],
        [gl_cv_func_ceil_libm="-lm"])
      LIBS="$save_LIBS"
    fi
  ])
  CEIL_LIBM="$gl_cv_func_ceil_libm"
])
