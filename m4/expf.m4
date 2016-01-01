# expf.m4 serial 2
dnl Copyright (C) 2011-2016 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_EXPF],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  AC_REQUIRE([gl_FUNC_EXP])

  dnl Persuade glibc <math.h> to declare expf().
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])

  dnl Test whether expf() exists. Assume that expf(), if it exists, is
  dnl defined in the same library as exp().
  save_LIBS="$LIBS"
  LIBS="$LIBS $EXP_LIBM"
  AC_CHECK_FUNCS([expf])
  LIBS="$save_LIBS"
  if test $ac_cv_func_expf = yes; then
    EXPF_LIBM="$EXP_LIBM"
  else
    HAVE_EXPF=0
    EXPF_LIBM="$EXP_LIBM"
  fi
  AC_SUBST([EXPF_LIBM])
])
