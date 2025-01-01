# timespec_getres.m4
# serial 2
dnl Copyright 2021-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN([gl_FUNC_TIMESPEC_GETRES],
[
  AC_REQUIRE([gl_TIME_H_DEFAULTS])

  dnl Might be needed for the same reason timespec_get needs it.
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])

  gl_CHECK_FUNCS_ANDROID([timespec_getres], [[#include <time.h>]])
  if test $ac_cv_func_timespec_getres != yes; then
    HAVE_TIMESPEC_GETRES=0
    case "$gl_cv_onwards_func_timespec_getres" in
      future*) REPLACE_TIMESPEC_GETRES=1 ;;
    esac
  fi
])
