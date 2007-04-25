# ftello.m4 serial 1
dnl Copyright (C) 2007 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_FTELLO],
[
  AC_REQUIRE([gl_STDIO_H_DEFAULTS])
  AC_REQUIRE([AC_PROG_CC])
  AC_CACHE_CHECK([for ftello], [gl_cv_func_ftello],
    [
      AC_TRY_LINK([#include <stdio.h>], [ftello (stdin);],
        [gl_cv_func_ftello=yes], [gl_cv_func_ftello=no])
    ])
  if test $gl_cv_func_ftello = no; then
    HAVE_FTELLO=0
  fi
])
