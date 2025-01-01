# setpayload.m4
# serial 2
dnl Copyright 2024-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN([gl_FUNC_SETPAYLOADF],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])

  gl_MATHFUNC([setpayloadf], [int], [(float *, float)])
  if test $gl_cv_func_setpayloadf_no_libm != yes \
     && test $gl_cv_func_setpayloadf_in_libm != yes; then
    HAVE_SETPAYLOADF=0
  fi
  if test $HAVE_SETPAYLOADF = 0; then
    SETPAYLOADF_LIBM=
    dnl Prerequisite of lib/setpayloadf.c.
    gl_NAN_MIPS
  fi
  AC_SUBST([SETPAYLOADF_LIBM])
])

AC_DEFUN_ONCE([gl_FUNC_SETPAYLOAD],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])

  gl_MATHFUNC([setpayload], [int], [(double *, double)])
  if test $gl_cv_func_setpayload_no_libm != yes \
     && test $gl_cv_func_setpayload_in_libm != yes; then
    HAVE_SETPAYLOAD=0
  fi
  if test $HAVE_SETPAYLOAD = 0; then
    SETPAYLOAD_LIBM=
    dnl Prerequisite of lib/setpayload.c.
    gl_NAN_MIPS
  fi
  AC_SUBST([SETPAYLOAD_LIBM])
])

AC_DEFUN([gl_FUNC_SETPAYLOADL],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])
  AC_REQUIRE([gl_LONG_DOUBLE_VS_DOUBLE])

  gl_MATHFUNC([setpayloadl], [int], [(long double *, long double)])
  if test $gl_cv_func_setpayloadl_no_libm != yes \
     && test $gl_cv_func_setpayloadl_in_libm != yes; then
    HAVE_SETPAYLOADL=0
  fi
  if test $HAVE_SETPAYLOADL = 0; then
    dnl Find libraries needed to link lib/setpayloadl.c.
    if test $HAVE_SAME_LONG_DOUBLE_AS_DOUBLE = 1; then
      AC_REQUIRE([gl_FUNC_SETPAYLOAD])
      SETPAYLOADL_LIBM="$SETPAYLOAD_LIBM"
    else
      SETPAYLOADL_LIBM=
    fi
    dnl Prerequisite of lib/setpayloadl.c.
    gl_LONG_DOUBLE_EXPONENT_LOCATION
    gl_NAN_MIPS
  fi
  AC_SUBST([SETPAYLOADL_LIBM])
])
