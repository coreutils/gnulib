# mkostemps.m4 serial 4
dnl Copyright (C) 2009-2023 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_MKOSTEMPS],
[
  AC_REQUIRE([gl_STDLIB_H_DEFAULTS])

  dnl Persuade glibc <stdlib.h> to declare mkostemps().
  AC_REQUIRE([AC_USE_SYSTEM_EXTENSIONS])

  gl_CHECK_FUNCS_ANDROID([mkostemps], [[#include <stdlib.h>]])
  if test $ac_cv_func_mkostemps != yes; then
    HAVE_MKOSTEMPS=0
    case "$gl_cv_onwards_func_mkostemps" in
      future*) REPLACE_MKOSTEMPS=1 ;;
    esac
  fi
])
