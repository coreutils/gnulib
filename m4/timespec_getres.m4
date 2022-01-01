# Test whether timespec_getres works.
dnl Copyright 2021-2022 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_TIMESPEC_GETRES],
[
  AC_REQUIRE([gl_TIME_H_DEFAULTS])

  dnl Might be needed for the same reason timespec_get needs it.
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])

  AC_CHECK_FUNCS_ONCE([timespec_getres])
  if test $ac_cv_func_timespec_getres != yes; then
    HAVE_TIMESPEC_GETRES=0
  fi
])
