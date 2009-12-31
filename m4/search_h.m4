# search_h.m4 serial 4
dnl Copyright (C) 2007-2010 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_SEARCH_H],
[
  AC_REQUIRE([gl_SEARCH_H_DEFAULTS])
  AC_CHECK_HEADERS_ONCE([search.h])
  gl_CHECK_NEXT_HEADERS([search.h])
  if test $ac_cv_header_search_h = yes; then
    HAVE_SEARCH_H=1
  else
    HAVE_SEARCH_H=0
  fi
  AC_SUBST([HAVE_SEARCH_H])
])

AC_DEFUN([gl_SEARCH_MODULE_INDICATOR],
[
  dnl Use AC_REQUIRE here, so that the default settings are expanded once only.
  AC_REQUIRE([gl_SEARCH_H_DEFAULTS])
  GNULIB_[]m4_translit([$1],[abcdefghijklmnopqrstuvwxyz./-],[ABCDEFGHIJKLMNOPQRSTUVWXYZ___])=1
])

AC_DEFUN([gl_SEARCH_H_DEFAULTS],
[
  GNULIB_TSEARCH=0; AC_SUBST([GNULIB_TSEARCH])
  dnl Assume proper GNU behavior unless another module says otherwise.
  HAVE_TSEARCH=1;    AC_SUBST([HAVE_TSEARCH])
  REPLACE_TSEARCH=0; AC_SUBST([REPLACE_TSEARCH])
])
