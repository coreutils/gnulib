# search_h.m4 serial 8
dnl Copyright (C) 2007-2016 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_SEARCH_H],
[
  AC_REQUIRE([gl_SEARCH_H_DEFAULTS])
  gl_CHECK_NEXT_HEADERS([search.h])
  if test $ac_cv_header_search_h = yes; then
    HAVE_SEARCH_H=1
  else
    HAVE_SEARCH_H=0
  fi
  AC_SUBST([HAVE_SEARCH_H])

  dnl Check for declarations of anything we want to poison if the
  dnl corresponding gnulib module is not in use.
  gl_WARN_ON_USE_PREPARE([[#include <search.h>
    ]], [tdelete tfind tsearch twalk])
])

AC_DEFUN([gl_SEARCH_MODULE_INDICATOR],
[
  dnl Use AC_REQUIRE here, so that the default settings are expanded once only.
  AC_REQUIRE([gl_SEARCH_H_DEFAULTS])
  gl_MODULE_INDICATOR_SET_VARIABLE([$1])
  dnl Define it also as a C macro, for the benefit of the unit tests.
  gl_MODULE_INDICATOR_FOR_TESTS([$1])
])

AC_DEFUN([gl_SEARCH_H_DEFAULTS],
[
  GNULIB_TSEARCH=0; AC_SUBST([GNULIB_TSEARCH])
  dnl Assume proper GNU behavior unless another module says otherwise.
  HAVE_TSEARCH=1;    AC_SUBST([HAVE_TSEARCH])
  REPLACE_TSEARCH=0; AC_SUBST([REPLACE_TSEARCH])
])
