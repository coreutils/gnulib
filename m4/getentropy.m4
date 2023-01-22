# getentropy.m4 serial 3
dnl Copyright 2020-2023 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl Written by Paul Eggert.

AC_DEFUN([gl_FUNC_GETENTROPY],
[
  AC_REQUIRE([gl_UNISTD_H_DEFAULTS])
  gl_CHECK_FUNCS_ANDROID([getentropy],
    [[#include <unistd.h>
      /* Additional includes are needed before <sys/random.h> on uClibc
         and Mac OS X.  */
      #include <sys/types.h>
      #include <stdlib.h>
      #include <sys/random.h>
    ]])
  if test $ac_cv_func_getentropy = no; then
    HAVE_GETENTROPY=0
    case "$gl_cv_onwards_func_getentropy" in
      future*) REPLACE_GETENTROPY=1 ;;
    esac
  fi
])
