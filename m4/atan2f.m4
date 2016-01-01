# atan2f.m4 serial 2
dnl Copyright (C) 2011-2016 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_ATAN2F],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  AC_REQUIRE([gl_FUNC_ATAN2])

  dnl Persuade glibc <math.h> to declare atan2f().
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])

  dnl Test whether atan2f() exists. Assume that atan2f(), if it exists, is
  dnl defined in the same library as atan2().
  save_LIBS="$LIBS"
  LIBS="$LIBS $ATAN2_LIBM"
  AC_CHECK_FUNCS([atan2f])
  LIBS="$save_LIBS"
  if test $ac_cv_func_atan2f = yes; then
    ATAN2F_LIBM="$ATAN2_LIBM"
  else
    HAVE_ATAN2F=0
    ATAN2F_LIBM="$ATAN2_LIBM"
  fi
  AC_SUBST([ATAN2F_LIBM])
])
