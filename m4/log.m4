# log.m4 serial 2
dnl Copyright (C) 2011-2012 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_LOG],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])

  dnl Determine LOG_LIBM.
  gl_COMMON_DOUBLE_MATHFUNC([log])

  save_LIBS="$LIBS"
  LIBS="$LIBS $LOG_LIBM"
  gl_FUNC_LOG_WORKS
  LIBS="$save_LIBS"
  case "$gl_cv_func_log_works" in
    *yes) ;;
    *) REPLACE_LOG=1 ;;
  esac
])

dnl Test whether log() works.
dnl On OSF/1 5.1, log(-0.0) is NaN.
AC_DEFUN([gl_FUNC_LOG_WORKS],
[
  AC_REQUIRE([AC_PROG_CC])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  AC_CACHE_CHECK([whether log works], [gl_cv_func_log_works],
    [
      AC_RUN_IFELSE(
        [AC_LANG_SOURCE([[
#include <math.h>
volatile double x;
double y;
int main ()
{
  x = -0.0;
  y = log (x);
  if (!(y + y == y))
    return 1;
  return 0;
}
]])],
        [gl_cv_func_log_works=yes],
        [gl_cv_func_log_works=no],
        [case "$host_os" in
           osf*) gl_cv_func_log_works="guessing no";;
           *)    gl_cv_func_log_works="guessing yes";;
         esac
        ])
    ])
])
