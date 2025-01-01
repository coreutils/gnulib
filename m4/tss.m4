# tss.m4
# serial 1
dnl Copyright (C) 2024-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN([gl_FUNC_TSS_CREATE],
[
  AC_REQUIRE([gl_THREADS_H])
  gl_CHECK_FUNCS_ANDROID([tss_create], [[#include <threads.h>]])
  case "$gl_cv_onwards_func_tss_create" in
    future*)
      REPLACE_TSS_CREATE=1
      dnl Assume that the other tss_* functions are introduced at the same time
      dnl as tss_create.
      REPLACE_TSS_DELETE=1
      REPLACE_TSS_GET=1
      REPLACE_TSS_SET=1
      ;;
  esac
])
