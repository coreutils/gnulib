# grantpt.m4 serial 2
dnl Copyright (C) 2010-2012 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_GRANTPT],
[
  AC_REQUIRE([gl_UNISTD_H_DEFAULTS])

  dnl Persuade glibc <stdlib.h> to declare grantpt().
  AC_REQUIRE([AC_USE_SYSTEM_EXTENSIONS])

  AC_CHECK_FUNCS([grantpt])
  if test $ac_cv_func_grantpt = no; then
    HAVE_GRANTPT=0
  fi
])

# Prerequisites of lib/grantpt.c.
AC_DEFUN([gl_PREREQ_GRANTPT], [
  AC_CHECK_FUNCS([setrlimit])
])
