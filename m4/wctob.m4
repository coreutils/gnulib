# wctob.m4 serial 1
dnl Copyright (C) 2008 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_WCTOB],
[
  AC_REQUIRE([gl_WCHAR_H_DEFAULTS])

  AC_CHECK_FUNCS_ONCE([wctob])
  if test $ac_cv_func_wctob = no; then
    HAVE_WCTOB=0
    gl_REPLACE_WCHAR_H
    AC_LIBOBJ([wctob])
    gl_PREREQ_WCTOB
  fi
])

# Prerequisites of lib/wctob.c.
AC_DEFUN([gl_PREREQ_WCTOB], [
  :
])
