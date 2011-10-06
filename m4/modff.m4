# modff.m4 serial 1
dnl Copyright (C) 2011 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_MODFF],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  AC_REQUIRE([gl_FUNC_MODF])

  dnl Test whether modff() exists. Assume that modff(), if it exists, is
  dnl defined in the same library as modf().
  save_LIBS="$LIBS"
  LIBS="$LIBS $MODF_LIBM"
  AC_CHECK_FUNCS([modff])
  LIBS="$save_LIBS"
  if test $ac_cv_func_modff = yes; then
    MODFF_LIBM="$MODF_LIBM"
  else
    HAVE_MODFF=0
    MODFF_LIBM="$MODF_LIBM"
  fi
  AC_SUBST([MODFF_LIBM])
])
