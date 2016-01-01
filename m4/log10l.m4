# log10l.m4 serial 3
dnl Copyright (C) 2011-2016 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_LOG10L],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  AC_REQUIRE([gl_LONG_DOUBLE_VS_DOUBLE])
  AC_REQUIRE([gl_FUNC_LOG10])

  dnl Persuade glibc <math.h> to declare log10l().
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])

  dnl Test whether log10l() is declared. On AIX 5.1 it is not declared.
  AC_CHECK_DECL([log10l], , [HAVE_DECL_LOG10L=0], [[#include <math.h>]])

  dnl Test whether log10l() exists. Assume that log10l(), if it exists, is
  dnl defined in the same library as log10().
  save_LIBS="$LIBS"
  LIBS="$LIBS $LOG10_LIBM"
  AC_CHECK_FUNCS([log10l])
  LIBS="$save_LIBS"
  if test $ac_cv_func_log10l = yes; then
    LOG10L_LIBM="$LOG10_LIBM"

    save_LIBS="$LIBS"
    LIBS="$LIBS $LOG10L_LIBM"
    gl_FUNC_LOG10L_WORKS
    LIBS="$save_LIBS"
    case "$gl_cv_func_log10l_works" in
      *yes) ;;
      *) REPLACE_LOG10L=1 ;;
    esac
  else
    HAVE_LOG10L=0
  fi
  if test $HAVE_LOG10L = 0 || test $REPLACE_LOG10L = 1; then
    if test $HAVE_SAME_LONG_DOUBLE_AS_DOUBLE = 1; then
      LOG10L_LIBM="$LOG10_LIBM"
    else
      if test $HAVE_LOG10L = 0; then
        AC_REQUIRE([gl_FUNC_LOGL])
        LOG10L_LIBM="$LOGL_LIBM"
      fi
    fi
  fi
  AC_SUBST([LOG10L_LIBM])
])

dnl Test whether log10l() works.
dnl On OSF/1 5.1, log10l(-0.0L) is NaN.
dnl On IRIX 6.5, log10l(-0.0L) is an unnormalized negative infinity
dnl 0xFFF00000000000007FF0000000000000, should be
dnl 0xFFF00000000000000000000000000000.
dnl On AIX 5.1, log10l(-0.0L) is finite if it's not the first log10l call
dnl in the program.
AC_DEFUN([gl_FUNC_LOG10L_WORKS],
[
  AC_REQUIRE([AC_PROG_CC])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  AC_CACHE_CHECK([whether log10l works], [gl_cv_func_log10l_works],
    [
      AC_RUN_IFELSE(
        [AC_LANG_SOURCE([[
#include <math.h>
#ifndef log10l /* for AIX */
extern
#ifdef __cplusplus
"C"
#endif
long double log10l (long double);
#endif
volatile long double x;
long double y;
int main ()
{
  /* Dummy call, to trigger the AIX 5.1 bug.  */
  x = 0.6L;
  y = log10l (x);
  /* This test fails on AIX 5.1, IRIX 6.5, OSF/1 5.1.  */
  x = -0.0L;
  y = log10l (x);
  if (!(y + y == y))
    return 1;
  return 0;
}
]])],
        [gl_cv_func_log10l_works=yes],
        [gl_cv_func_log10l_works=no],
        [case "$host_os" in
           aix* | irix* | osf*) gl_cv_func_log10l_works="guessing no";;
           *)                   gl_cv_func_log10l_works="guessing yes";;
         esac
        ])
    ])
])
