# raise.m4 serial 1
dnl Copyright (C) 2011 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_RAISE],
[
  AC_REQUIRE([gl_SIGNAL_H_DEFAULTS])
  AC_REQUIRE([AC_CANONICAL_HOST])
  AC_REQUIRE([gl_MSVC_INVAL])
  AC_CHECK_FUNCS([raise])
  if test $ac_cv_func_raise = no; then
    HAVE_RAISE=0
  else
    if test $HAVE_MSVC_INVALID_PARAMETER_HANDLER = 1; then
      REPLACE_RAISE=1
    fi
  fi
])

# Prerequisites of lib/raise.c.
AC_DEFUN([gl_PREREQ_RAISE], [
  AC_REQUIRE([AC_C_INLINE])
])
