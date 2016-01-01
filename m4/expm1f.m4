# expm1f.m4 serial 2
dnl Copyright (C) 2011-2016 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_EXPM1F],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  AC_REQUIRE([gl_FUNC_EXPM1])

  dnl Persuade glibc <math.h> to declare expm1f().
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])

  dnl Test whether expm1f() exists. Assume that expm1f(), if it exists, is
  dnl defined in the same library as expm1().
  save_LIBS="$LIBS"
  LIBS="$LIBS $EXPM1_LIBM"
  AC_CHECK_FUNCS([expm1f])
  LIBS="$save_LIBS"
  if test $ac_cv_func_expm1f = yes; then
    EXPM1F_LIBM="$EXPM1_LIBM"
    save_LIBS="$LIBS"
    LIBS="$LIBS $EXPM1F_LIBM"
    gl_FUNC_EXPM1F_WORKS
    LIBS="$save_LIBS"
    case "$gl_cv_func_expm1f_works" in
      *yes) ;;
      *) REPLACE_EXPM1F=1 ;;
    esac
  else
    HAVE_EXPM1F=0
  fi
  if test $HAVE_EXPM1F = 0 || test $REPLACE_EXPM1F = 1; then
    dnl Find libraries needed to link lib/expm1f.c.
    EXPM1F_LIBM="$EXPM1_LIBM"
  fi
  AC_SUBST([EXPM1F_LIBM])
])

dnl Test whether expm1f() works.
dnl On IRIX 6.5, for arguments <= -17.32868, it returns -5.6295e14.
AC_DEFUN([gl_FUNC_EXPM1F_WORKS],
[
  AC_REQUIRE([AC_PROG_CC])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  AC_CACHE_CHECK([whether expm1f works], [gl_cv_func_expm1f_works],
    [
      AC_RUN_IFELSE(
        [AC_LANG_SOURCE([[
#include <math.h>
volatile float x;
float y;
int main ()
{
  x = -100.0f;
  y = expm1f (x);
  if (y < -1.0f)
    return 1;
  return 0;
}
]])],
        [gl_cv_func_expm1f_works=yes],
        [gl_cv_func_expm1f_works=no],
        [case "$host_os" in
           irix*) gl_cv_func_expm1f_works="guessing no";;
           *)     gl_cv_func_expm1f_works="guessing yes";;
         esac
        ])
    ])
])
