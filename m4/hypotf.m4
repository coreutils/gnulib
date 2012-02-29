# hypotf.m4 serial 1
dnl Copyright (C) 2012 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_HYPOTF],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  AC_REQUIRE([gl_FUNC_HYPOT])

  dnl Test whether hypotf() exists. Assume that hypotf(), if it exists, is
  dnl defined in the same library as hypot().
  save_LIBS="$LIBS"
  LIBS="$LIBS $HYPOT_LIBM"
  AC_CHECK_FUNCS([hypotf])
  LIBS="$save_LIBS"
  if test $ac_cv_func_hypotf = yes; then
    HYPOTF_LIBM="$HYPOT_LIBM"
    save_LIBS="$LIBS"
    LIBS="$LIBS $HYPOTF_LIBM"
    gl_FUNC_HYPOTF_WORKS
    LIBS="$save_LIBS"
    case "$gl_cv_func_hypotf_works" in
      *yes) ;;
      *) REPLACE_HYPOTF=1 ;;
    esac
  else
    HAVE_HYPOTF=0
  fi
  if test $HAVE_HYPOTF = 0 || test $REPLACE_HYPOTF = 1; then
    dnl Find libraries needed to link lib/hypotf.c.
    HYPOTF_LIBM="$HYPOT_LIBM"
  fi
  AC_SUBST([HYPOTF_LIBM])
])

dnl Test whether hypotf() works.
dnl It returns wrong values on NetBSD 5.1/x86_64 and OpenBSD 4.9/x86.
AC_DEFUN([gl_FUNC_HYPOTF_WORKS],
[
  AC_REQUIRE([AC_PROG_CC])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  AC_CACHE_CHECK([whether hypotf works], [gl_cv_func_hypotf_works],
    [
      AC_RUN_IFELSE(
        [AC_LANG_SOURCE([[
#include <float.h>
#include <math.h>
volatile float x;
volatile float y;
volatile float z;
int main ()
{
  int result = 0;
  /* This test fails on NetBSD 5.1.  */
  {
    x = FLT_MIN * 2.0f;
    y = FLT_MIN * 3.0f;
    z = hypotf (x, y);
    if (!(z >= FLT_MIN * 2.0f && z <= FLT_MIN * 4.0f))
      result |= 1;
  }
  /* This test fails on OpenBSD 4.9.  */
  {
    x = FLT_MAX;
    y = FLT_MAX * 0.5f;
    z = hypotf (x, y);
    if (!(z > 0 && z == z + z))
      result |= 2;
  }
  return result;
}
]])],
        [gl_cv_func_hypotf_works=yes],
        [gl_cv_func_hypotf_works=no],
        [case "$host_os" in
           netbsd* | openbsd*) gl_cv_func_hypotf_works="guessing no";;
           *)                  gl_cv_func_hypotf_works="guessing yes";;
         esac
        ])
    ])
])
