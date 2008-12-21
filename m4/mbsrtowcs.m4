# mbsrtowcs.m4 serial 2
dnl Copyright (C) 2008 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_MBSRTOWCS],
[
  AC_REQUIRE([gl_WCHAR_H_DEFAULTS])

  AC_REQUIRE([AC_TYPE_MBSTATE_T])
  gl_MBSTATE_T_BROKEN
  if test $REPLACE_MBSTATE_T = 1; then
    REPLACE_MBSRTOWCS=1
  fi
  AC_CHECK_FUNCS_ONCE([mbsrtowcs])
  if test $ac_cv_func_mbsrtowcs = no; then
    HAVE_MBSRTOWCS=0
  fi
  if test $HAVE_MBSRTOWCS = 0 || test $REPLACE_MBSRTOWCS = 1; then
    gl_REPLACE_WCHAR_H
    AC_LIBOBJ([mbsrtowcs])
    gl_PREREQ_MBSRTOWCS
  fi
])

# Prerequisites of lib/mbsrtowcs.c.
AC_DEFUN([gl_PREREQ_MBSRTOWCS], [
  :
])
