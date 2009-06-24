# fpurge.m4 serial 3
dnl Copyright (C) 2007, 2009 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_FPURGE],
[
  AC_REQUIRE([gl_STDIO_H_DEFAULTS])
  AC_CHECK_FUNCS_ONCE([fpurge])
  AC_CHECK_FUNCS_ONCE([__fpurge])
  AC_CHECK_DECLS([fpurge], , , [[#include <stdio.h>]])
  dnl For now, lib/fpurge.c is always compiled.
  if test "x$ac_cv_func_fpurge" = xyes; then
    REPLACE_FPURGE=1
  fi
  if test "x$ac_cv_have_decl_fpurge" = xno; then
    HAVE_DECL_FPURGE=0
  fi
])
