# ttyname_r.m4 serial 1
dnl Copyright (C) 2010 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_TTYNAME_R],
[
  AC_REQUIRE([gl_UNISTD_H_DEFAULTS])

  AC_CHECK_FUNCS([ttyname_r])
  if test $ac_cv_func_ttyname_r = no; then
    HAVE_TTYNAME_R=0
    AC_LIBOBJ([ttyname_r])
    gl_PREREQ_TTYNAME_R
  fi
])

# Prerequisites of lib/ttyname_r.c.
AC_DEFUN([gl_PREREQ_TTYNAME_R], [
  AC_CHECK_FUNCS([ttyname])
])
