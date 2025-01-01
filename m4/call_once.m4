# call_once.m4
# serial 2
dnl Copyright (C) 2024-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN([gl_FUNC_CALL_ONCE],
[
  AC_REQUIRE([gl_THREADS_H])
  AC_REQUIRE([AC_CANONICAL_HOST])
  gl_CHECK_FUNCS_ANDROID([call_once], [[#include <threads.h>]])
  case "$gl_cv_onwards_func_call_once" in
    yes)
      dnl Work around Cygwin 3.5.3 bug.
      AC_CACHE_CHECK([whether call_once works],
        [gl_cv_func_call_once_works],
        [case "$host_os" in
           cygwin*) gl_cv_func_call_once_works="guessing no" ;;
           *)       gl_cv_func_call_once_works="yes" ;;
         esac
        ])
      case "$gl_cv_func_call_once_works" in
        *yes) ;;
        *) REPLACE_CALL_ONCE=1 ;;
      esac
      ;;
    future*)
      REPLACE_CALL_ONCE=1
      ;;
  esac
])
