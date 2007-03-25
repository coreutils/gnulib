# tsearch.m4 serial 2
dnl Copyright (C) 2006-2007 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_TSEARCH],
[
  AC_REQUIRE([gl_SEARCH_H_DEFAULTS])
  AC_CHECK_FUNCS([tsearch])
  if test $ac_cv_func_tsearch = no; then
    HAVE_TSEARCH=0
    AC_LIBOBJ([tsearch])
    gl_PREREQ_TSEARCH
  fi
])

# Prerequisites of lib/tsearch.c.
AC_DEFUN([gl_PREREQ_TSEARCH], [
  :
])
