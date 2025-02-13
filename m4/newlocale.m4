# newlocale.m4
# serial 1
dnl Copyright (C) 2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN([gl_FUNC_NEWLOCALE],
[
  AC_REQUIRE([gl_LOCALE_H_DEFAULTS])
  gl_CHECK_FUNCS_ANDROID([newlocale], [[#include <locale.h>]])
  if test $ac_cv_func_newlocale = no; then
    HAVE_NEWLOCALE=0
  fi
])

# Prerequisites of lib/newlocale.c.
AC_DEFUN([gl_PREREQ_NEWLOCALE],
[
  :
])
