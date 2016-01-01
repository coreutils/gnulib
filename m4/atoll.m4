# atoll.m4 serial 2
dnl Copyright (C) 2008-2016 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_ATOLL],
[
  AC_REQUIRE([gl_STDLIB_H_DEFAULTS])
  dnl We don't need (and can't compile) the replacement strtoll
  dnl unless the type 'long long int' exists.
  AC_REQUIRE([AC_TYPE_LONG_LONG_INT])
  if test "$ac_cv_type_long_long_int" = yes; then
    AC_CHECK_FUNCS([atoll])
    if test $ac_cv_func_atoll = no; then
      HAVE_ATOLL=0
    fi
  fi
])

# Prerequisites of lib/atoll.c.
AC_DEFUN([gl_PREREQ_ATOLL], [
  :
])
