# getsubopt.m4 serial 7
dnl Copyright (C) 2004, 2007, 2009-2023 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_GETSUBOPT],
[
  dnl Persuade glibc <stdlib.h> to declare getsubopt().
  AC_REQUIRE([AC_USE_SYSTEM_EXTENSIONS])

  AC_REQUIRE([gl_STDLIB_H_DEFAULTS])
  gl_CHECK_FUNCS_ANDROID([getsubopt],
    [[#include <stdlib.h>
      #include <unistd.h>
    ]])
  if test $ac_cv_func_getsubopt = no; then
    HAVE_GETSUBOPT=0
    case "$gl_cv_onwards_func_getsubopt" in
      future*) REPLACE_GETSUBOPT=1 ;;
    esac
  fi
])

# Prerequisites of lib/getsubopt.c.
AC_DEFUN([gl_PREREQ_GETSUBOPT], [:])
