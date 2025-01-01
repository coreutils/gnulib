# mtx.m4
# serial 1
dnl Copyright (C) 2024-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN([gl_FUNC_MTX_INIT],
[
  AC_REQUIRE([gl_THREADS_H])
  gl_CHECK_FUNCS_ANDROID([mtx_init], [[#include <threads.h>]])
  case "$gl_cv_onwards_func_mtx_init" in
    future*)
      REPLACE_MTX_INIT=1
      dnl Assume that the other mtx_* functions are introduced at the same time
      dnl as mtx_init.
      REPLACE_MTX_DESTROY=1
      REPLACE_MTX_LOCK=1
      REPLACE_MTX_TIMEDLOCK=1
      REPLACE_MTX_TRYLOCK=1
      REPLACE_MTX_UNLOCK=1
      ;;
  esac
])
