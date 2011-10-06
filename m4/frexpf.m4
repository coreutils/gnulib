# frexpf.m4 serial 1
dnl Copyright (C) 2011 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_FREXPF],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  AC_REQUIRE([gl_FUNC_FREXP])

  dnl Test whether frexpf() exists. Assume that frexpf(), if it exists, is
  dnl defined in the same library as frexp().
  save_LIBS="$LIBS"
  LIBS="$LIBS $FREXP_LIBM"
  AC_CHECK_FUNCS([frexpf])
  LIBS="$save_LIBS"
  if test $ac_cv_func_frexpf = yes; then
    FREXPF_LIBM="$FREXP_LIBM"
  else
    HAVE_FREXPF=0
    FREXPF_LIBM="$FREXP_LIBM"
  fi
  AC_SUBST([FREXPF_LIBM])
])
