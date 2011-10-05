# fabsf.m4 serial 1
dnl Copyright (C) 2011 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_FABSF],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  AC_REQUIRE([gl_FUNC_FABS])

  dnl Test whether fabsf() exists. Assume that fabsf(), if it exists, is
  dnl defined in the same library as fabs().
  save_LIBS="$LIBS"
  LIBS="$LIBS $FABS_LIBM"
  AC_CHECK_FUNCS([fabsf])
  LIBS="$save_LIBS"
  if test $ac_cv_func_fabsf = yes; then
    FABSF_LIBM="$FABS_LIBM"
  else
    HAVE_FABSF=0
    FABSF_LIBM="$FABS_LIBM"
  fi
  AC_SUBST([FABSF_LIBM])
])
