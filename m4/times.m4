# times.m4 serial 1
dnl Copyright (C) 2009, 2010 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_TIMES],
[
  AC_REQUIRE([gl_SYS_TIMES_H_DEFAULTS])
  AC_CHECK_FUNCS_ONCE([times])
  if test $ac_cv_func_times = no; then
    HAVE_TIMES=0
    AC_LIBOBJ([times])
  fi
])
