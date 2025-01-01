# cnd.m4
# serial 1
dnl Copyright (C) 2024-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN([gl_FUNC_CND_INIT],
[
  AC_REQUIRE([gl_THREADS_H])
  gl_CHECK_FUNCS_ANDROID([cnd_init], [[#include <threads.h>]])
  case "$gl_cv_onwards_func_cnd_init" in
    future*)
      REPLACE_CND_INIT=1
      dnl Assume that the other cnd_* functions are introduced at the same time
      dnl as cnd_init.
      REPLACE_CND_BROADCAST=1
      REPLACE_CND_DESTROY=1
      REPLACE_CND_SIGNAL=1
      REPLACE_CND_TIMEDWAIT=1
      REPLACE_CND_WAIT=1
      ;;
  esac
])
