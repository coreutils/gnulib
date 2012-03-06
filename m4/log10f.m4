# log10f.m4 serial 2
dnl Copyright (C) 2011-2012 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_LOG10F],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  AC_REQUIRE([gl_FUNC_LOG10])

  dnl Persuade glibc <math.h> to declare log10f().
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])

  dnl Test whether log10f() exists. Assume that log10f(), if it exists, is
  dnl defined in the same library as log10().
  save_LIBS="$LIBS"
  LIBS="$LIBS $LOG10_LIBM"
  AC_CHECK_FUNCS([log10f])
  LIBS="$save_LIBS"
  if test $ac_cv_func_log10f = yes; then
    LOG10F_LIBM="$LOG10_LIBM"
  else
    HAVE_LOG10F=0
    LOG10F_LIBM="$LOG10_LIBM"
  fi
  AC_SUBST([LOG10F_LIBM])
])
