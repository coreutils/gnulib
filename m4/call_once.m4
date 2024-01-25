# call_once.m4 serial 1
dnl Copyright (C) 2024 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_CALL_ONCE],
[
  AC_REQUIRE([gl_THREADS_H])
  gl_CHECK_FUNCS_ANDROID([call_once], [[#include <threads.h>]])
  case "$gl_cv_onwards_func_call_once" in
    future*)
      REPLACE_CALL_ONCE=1
      ;;
  esac
])
