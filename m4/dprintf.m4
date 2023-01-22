# dprintf.m4 serial 3
dnl Copyright (C) 2009-2023 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_DPRINTF],
[
  AC_REQUIRE([gl_STDIO_H_DEFAULTS])
  gl_CHECK_FUNCS_ANDROID([dprintf], [[#include <stdio.h>]])
  if test $ac_cv_func_dprintf = no; then
    HAVE_DPRINTF=0
    gl_REPLACE_DPRINTF
  fi
])

AC_DEFUN([gl_REPLACE_DPRINTF],
[
  AC_REQUIRE([gl_STDIO_H_DEFAULTS])
  AC_LIBOBJ([dprintf])
  if test $ac_cv_func_dprintf = yes \
     || case "$gl_cv_onwards_func_dprintf" in \
          future*) true ;; \
          *) false ;; \
        esac; then
    REPLACE_DPRINTF=1
  fi
  gl_PREREQ_DPRINTF
])

# Prerequisites of lib/dprintf.c.
AC_DEFUN([gl_PREREQ_DPRINTF], [:])
