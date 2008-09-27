# iconv_h.m4 serial 4
dnl Copyright (C) 2007-2008 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_ICONV_H],
[
  AC_REQUIRE([gl_ICONV_H_DEFAULTS])
  gl_CHECK_NEXT_HEADERS([iconv.h])
])

dnl Unconditionally enables the replacement of <iconv.h>.
AC_DEFUN([gl_REPLACE_ICONV_H],
[
  AC_REQUIRE([gl_ICONV_H_DEFAULTS])
  ICONV_H='iconv.h'
])

AC_DEFUN([gl_ICONV_MODULE_INDICATOR],
[
  dnl Use AC_REQUIRE here, so that the default settings are expanded once only.
  AC_REQUIRE([gl_ICONV_H_DEFAULTS])
  GNULIB_[]m4_translit([$1],[abcdefghijklmnopqrstuvwxyz./-],[ABCDEFGHIJKLMNOPQRSTUVWXYZ___])=1
])

AC_DEFUN([gl_ICONV_H_DEFAULTS],
[
  dnl Assume proper GNU behavior unless another module says otherwise.
  REPLACE_ICONV=0;      AC_SUBST([REPLACE_ICONV])
  REPLACE_ICONV_OPEN=0; AC_SUBST([REPLACE_ICONV_OPEN])
  REPLACE_ICONV_UTF=0;  AC_SUBST([REPLACE_ICONV_UTF])
  ICONV_H='';           AC_SUBST([ICONV_H])
])
