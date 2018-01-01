# random.m4 serial 2
dnl Copyright (C) 2012-2018 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_RANDOM],
[
  AC_REQUIRE([gl_STDLIB_H_DEFAULTS])

  AC_CHECK_FUNCS([random])
  if test $ac_cv_func_random = no; then
    HAVE_RANDOM=0
  fi

  AC_CHECK_DECLS_ONCE([initstate])
  if test $ac_cv_have_decl_initstate = no; then
    HAVE_DECL_INITSTATE=0
  fi

  AC_CHECK_DECLS_ONCE([setstate])
  if test $ac_cv_have_decl_setstate = no; then
    HAVE_DECL_SETSTATE=0
  fi
])

# Prerequisites of lib/random.c.
AC_DEFUN([gl_PREREQ_RANDOM], [
  :
])
