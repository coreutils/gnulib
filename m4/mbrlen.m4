# mbrlen.m4 serial 1
dnl Copyright (C) 2008 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_MBRLEN],
[
  AC_REQUIRE([gl_WCHAR_H_DEFAULTS])

  AC_REQUIRE([AC_TYPE_MBSTATE_T])
  AC_CHECK_FUNCS_ONCE([mbrlen])
  if test $ac_cv_func_mbrlen = no; then
    HAVE_MBRLEN=0
    gl_REPLACE_WCHAR_H
    AC_LIBOBJ([mbrlen])
    gl_PREREQ_MBRLEN
  fi
])

# Prerequisites of lib/mbrlen.c.
AC_DEFUN([gl_PREREQ_MBRLEN], [
  :
])
