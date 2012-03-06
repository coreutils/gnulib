# expm1f.m4 serial 1
dnl Copyright (C) 2011-2012 Free Software Foundation, Inc.
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
  else
    HAVE_EXPM1F=0
    dnl Find libraries needed to link lib/expm1f.c.
    EXPM1F_LIBM="$EXPM1_LIBM"
  fi
  AC_SUBST([EXPM1F_LIBM])
])
