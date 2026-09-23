# alignalloc.m4
# serial 2
dnl Copyright 2022-2026 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN([gl_ALIGNALLOC],
[
  AC_REQUIRE([AC_USE_SYSTEM_EXTENSIONS])
  gl_FUNC_ALIGNED_ALLOC_WORKS
  case "$gl_cv_func_aligned_alloc_works" in
    *yes)
      AC_DEFINE([HAVE_WORKING_ALIGNED_ALLOC], [1],
        [Define to 1 if aligned_alloc exists and works.])
      ;;
  esac
  gl_CHECK_FUNCS_ANDROID([posix_memalign], [[#include <stdlib.h>]])
])
