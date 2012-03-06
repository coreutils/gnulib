# log10l.m4 serial 2
dnl Copyright (C) 2011-2012 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_LOG10L],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  AC_REQUIRE([gl_LONG_DOUBLE_VS_DOUBLE])
  AC_REQUIRE([gl_FUNC_LOG10])

  dnl Persuade glibc <math.h> to declare log10l().
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])

  dnl Test whether log10l() is declared. On AIX 5.1 it is not declared.
  AC_CHECK_DECL([log10l], , [HAVE_DECL_LOG10L=0], [[#include <math.h>]])

  dnl Test whether log10l() exists. Assume that log10l(), if it exists, is
  dnl defined in the same library as log10().
  save_LIBS="$LIBS"
  LIBS="$LIBS $LOG10_LIBM"
  AC_CHECK_FUNCS([log10l])
  LIBS="$save_LIBS"
  if test $ac_cv_func_log10l = yes; then
    LOG10L_LIBM="$LOG10_LIBM"
  else
    HAVE_LOG10L=0
    if test $HAVE_SAME_LONG_DOUBLE_AS_DOUBLE = 1; then
      LOG10L_LIBM="$LOG10_LIBM"
    else
      AC_REQUIRE([gl_FUNC_LOGL])
      LOG10L_LIBM="$LOGL_LIBM"
    fi
  fi
  AC_SUBST([LOG10L_LIBM])
])
