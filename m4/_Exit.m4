# _Exit.m4 serial 4
dnl Copyright (C) 2010-2023 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC__EXIT],
[
  AC_REQUIRE([gl_STDLIB_H_DEFAULTS])
  gl_CHECK_FUNCS_ANDROID([_Exit], [[#include <stdlib.h>]])
  if test $ac_cv_func__Exit = no; then
    HAVE__EXIT=0
    case "$gl_cv_onwards_func__Exit" in
      future*) REPLACE__EXIT=1 ;;
    esac
  fi
])

# Prerequisites of lib/_Exit.c.
AC_DEFUN([gl_PREREQ__EXIT], [
  :
])
