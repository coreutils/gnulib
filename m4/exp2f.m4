# exp2f.m4
# serial 3
dnl Copyright (C) 2011-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN([gl_FUNC_EXP2F],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  AC_REQUIRE([gl_FUNC_EXP2])

  dnl Persuade glibc <math.h> to declare exp2f().
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])

  dnl Test whether exp2f() exists. Assume that exp2f(), if it exists, is
  dnl defined in the same library as exp2().
  saved_LIBS="$LIBS"
  LIBS="$LIBS $EXP2_LIBM"
  AC_CHECK_FUNCS([exp2f])
  LIBS="$saved_LIBS"
  if test $ac_cv_func_exp2f = yes; then
    HAVE_EXP2F=1
    EXP2F_LIBM="$EXP2_LIBM"
  else
    HAVE_EXP2F=0
    dnl Find libraries needed to link lib/exp2f.c.
    EXP2F_LIBM="$EXP2_LIBM"
  fi
  AC_SUBST([EXP2F_LIBM])
])
