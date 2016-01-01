# ffsll.m4 serial 1
dnl Copyright (C) 2011-2016 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_FFSLL],
[
  AC_REQUIRE([gl_HEADER_STRING_H_DEFAULTS])

  dnl Persuade glibc <string.h> to declare ffsll().
  AC_REQUIRE([AC_USE_SYSTEM_EXTENSIONS])

  AC_CHECK_FUNCS_ONCE([ffsll])
  if test $ac_cv_func_ffsll = no; then
    HAVE_FFSLL=0
  fi
])
