# isprint_l.m4
# serial 1
dnl Copyright (C) 2009-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN([gl_FUNC_ISPRINT_L],
[
  AC_REQUIRE([gl_CTYPE_H_DEFAULTS])

  dnl Persuade glibc <ctype.h> to declare isprint_l().
  AC_REQUIRE([AC_USE_SYSTEM_EXTENSIONS])

  AC_CHECK_FUNCS_ONCE([isprint_l])
  if test $ac_cv_func_isprint_l = no; then
    HAVE_ISPRINT_L=0
  fi

  dnl Prerequisites of lib/isprint_l.c.
  AC_REQUIRE([gt_FUNC_USELOCALE])
])
