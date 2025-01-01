# timespec_get.m4
# serial 6
dnl Copyright (C) 2021-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN([gl_FUNC_TIMESPEC_GET],
[
  AC_REQUIRE([gl_TIME_H_DEFAULTS])

  dnl Persuade OpenBSD <time.h> to declare timespec_get().
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])

  AC_REQUIRE([gl_CHECK_FUNC_TIMESPEC_GET])
  if test $gl_cv_func_timespec_get != yes; then
    HAVE_TIMESPEC_GET=0
    case "$gl_cv_onwards_func_timespec_get" in
      future*) REPLACE_TIMESPEC_GET=1 ;;
    esac
  fi
])
