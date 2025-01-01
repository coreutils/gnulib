# setpayloadsig.m4
# serial 1
dnl Copyright 2024-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN([gl_FUNC_SETPAYLOADSIGF],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])

  gl_MATHFUNC([setpayloadsigf], [int], [(float *, float)])
  if test $gl_cv_func_setpayloadsigf_no_libm != yes \
     && test $gl_cv_func_setpayloadsigf_in_libm != yes; then
    HAVE_SETPAYLOADSIGF=0
  fi
  if test $HAVE_SETPAYLOADSIGF = 0; then
    SETPAYLOADSIGF_LIBM=
  fi
  AC_SUBST([SETPAYLOADSIGF_LIBM])
])

AC_DEFUN_ONCE([gl_FUNC_SETPAYLOADSIG],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])

  gl_MATHFUNC([setpayloadsig], [int], [(double *, double)])
  if test $gl_cv_func_setpayloadsig_no_libm != yes \
     && test $gl_cv_func_setpayloadsig_in_libm != yes; then
    HAVE_SETPAYLOADSIG=0
  fi
  if test $HAVE_SETPAYLOADSIG = 0; then
    SETPAYLOADSIG_LIBM=
  fi
  AC_SUBST([SETPAYLOADSIG_LIBM])
])

AC_DEFUN([gl_FUNC_SETPAYLOADSIGL],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])
  AC_REQUIRE([gl_LONG_DOUBLE_VS_DOUBLE])

  gl_MATHFUNC([setpayloadsigl], [int], [(long double *, long double)])
  if test $gl_cv_func_setpayloadsigl_no_libm != yes \
     && test $gl_cv_func_setpayloadsigl_in_libm != yes; then
    HAVE_SETPAYLOADSIGL=0
  fi
  if test $HAVE_SETPAYLOADSIGL = 0; then
    dnl Find libraries needed to link lib/setpayloadsigl.c.
    if test $HAVE_SAME_LONG_DOUBLE_AS_DOUBLE = 1; then
      AC_REQUIRE([gl_FUNC_SETPAYLOADSIG])
      SETPAYLOADSIGL_LIBM="$SETPAYLOADSIG_LIBM"
    else
      SETPAYLOADSIGL_LIBM=
    fi
    dnl Prerequisite of lib/setpayloadsigl.c.
    gl_LONG_DOUBLE_EXPONENT_LOCATION
  fi
  AC_SUBST([SETPAYLOADSIGL_LIBM])
])
