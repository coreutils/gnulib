# strptime.m4 serial 5
dnl Copyright (C) 2007, 2009 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_STRPTIME],
[
  AC_REQUIRE([gl_HEADER_TIME_H_DEFAULTS])
  AC_REQUIRE([AC_C_RESTRICT])
  AC_REPLACE_FUNCS([strptime])
  AC_REQUIRE([gl_TM_GMTOFF])
  if test $ac_cv_func_strptime = yes; then
    REPLACE_STRPTIME=0
  else
    REPLACE_STRPTIME=1
  fi
])
