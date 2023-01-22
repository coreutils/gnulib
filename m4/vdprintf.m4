# vdprintf.m4 serial 3
dnl Copyright (C) 2009-2023 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_VDPRINTF],
[
  AC_REQUIRE([gl_STDIO_H_DEFAULTS])
  gl_CHECK_FUNCS_ANDROID([vdprintf], [[#include <stdio.h>]])
  if test $ac_cv_func_vdprintf = no; then
    HAVE_VDPRINTF=0
    gl_REPLACE_VDPRINTF
  fi
])

AC_DEFUN([gl_REPLACE_VDPRINTF],
[
  AC_REQUIRE([gl_STDIO_H_DEFAULTS])
  AC_LIBOBJ([vdprintf])
  if test $ac_cv_func_vdprintf = yes \
     || case "$gl_cv_onwards_func_vdprintf" in \
          future*) true ;; \
          *) false ;; \
        esac; then
    REPLACE_VDPRINTF=1
  fi
  gl_PREREQ_VDPRINTF
])

# Prerequisites of lib/vdprintf.c.
AC_DEFUN([gl_PREREQ_VDPRINTF], [:])
