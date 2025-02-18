# strerror_l.m4
# serial 1
dnl Copyright (C) 2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN([gl_FUNC_STRERROR_L],
[
  AC_REQUIRE([gl_STRING_H_DEFAULTS])

  dnl Persuade glibc <string.h> to declare strerror_l().
  AC_REQUIRE([AC_USE_SYSTEM_EXTENSIONS])

  AC_CHECK_FUNCS_ONCE([strerror_l])
  if test $ac_cv_func_strerror_l = no; then
    HAVE_STRERROR_L=0
  fi
])

# Prerequisites of lib/strerror_l.c.
AC_DEFUN([gl_PREREQ_STRERROR_L], [
  AC_REQUIRE([gt_FUNC_USELOCALE])
  :
])
