# logf.m4 serial 1
dnl Copyright (C) 2011 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_LOGF],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  AC_REQUIRE([gl_FUNC_LOG])

  dnl Test whether logf() exists. Assume that logf(), if it exists, is
  dnl defined in the same library as log().
  save_LIBS="$LIBS"
  LIBS="$LIBS $LOG_LIBM"
  AC_CHECK_FUNCS([logf])
  LIBS="$save_LIBS"
  if test $ac_cv_func_logf = yes; then
    LOGF_LIBM="$LOG_LIBM"
  else
    HAVE_LOGF=0
    LOGF_LIBM="$LOG_LIBM"
  fi
  AC_SUBST([LOGF_LIBM])
])
