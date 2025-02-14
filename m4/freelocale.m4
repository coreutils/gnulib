# freelocale.m4
# serial 2
dnl Copyright (C) 2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN([gl_FUNC_FREELOCALE],
[
  AC_REQUIRE([gl_LOCALE_H_DEFAULTS])
  AC_REQUIRE([gl_LOCALE_T])
  if test $HAVE_LOCALE_T = 1; then
    gl_CHECK_FUNCS_ANDROID([freelocale], [[#include <locale.h>]])
    gl_func_freelocale="$ac_cv_func_freelocale"
  else
    dnl In 2019, some versions of z/OS lack the locale_t type and have broken
    dnl newlocale, duplocale, freelocale functions.
    gl_cv_onwards_func_freelocale='future OS version'
    gl_func_freelocale=no
  fi
  if test $gl_func_freelocale != yes; then
    HAVE_FREELOCALE=0
    case "$gl_cv_onwards_func_freelocale" in
      future*) REPLACE_FREELOCALE=1 ;;
    esac
  fi
])

# Prerequisites of lib/freelocale.c.
AC_DEFUN([gl_PREREQ_FREELOCALE],
[
  :
])
