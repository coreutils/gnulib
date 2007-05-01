# sleep.m4 serial 1
dnl Copyright (C) 2007 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_SLEEP],
[
  AC_REQUIRE([gl_UNISTD_H_DEFAULTS])
  AC_CHECK_FUNCS_ONCE([sleep])
  if test $ac_cv_func_sleep = no; then
    HAVE_SLEEP=0
    AC_LIBOBJ([sleep])
    gl_PREREQ_SLEEP
  fi
])

# Prerequisites of lib/sleep.c.
AC_DEFUN([gl_PREREQ_SLEEP], [:])
