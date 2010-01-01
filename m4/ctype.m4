# ctype_h.m4 serial 2
dnl Copyright (C) 2009, 2010 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_CTYPE_H],
[
  AC_REQUIRE([gl_CTYPE_H_DEFAULTS])
  dnl Execute this unconditionally, because CTYPE_H may be set by other
  dnl modules, after this code is executed.
  gl_CHECK_NEXT_HEADERS([ctype.h])
])

AC_DEFUN([gl_CTYPE_MODULE_INDICATOR],
[
  dnl Use AC_REQUIRE here, so that the default settings are expanded once only.
  AC_REQUIRE([gl_CTYPE_H_DEFAULTS])
  GNULIB_[]m4_translit([$1],[abcdefghijklmnopqrstuvwxyz./-],[ABCDEFGHIJKLMNOPQRSTUVWXYZ___])=1
])

AC_DEFUN([gl_CTYPE_H_DEFAULTS],
[
  GNULIB_ISBLANK=0; AC_SUBST([GNULIB_ISBLANK])
  dnl Assume proper GNU behavior unless another module says otherwise.
  HAVE_ISBLANK=1;   AC_SUBST([HAVE_ISBLANK])
])
