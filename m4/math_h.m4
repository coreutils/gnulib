# math_h.m4 serial 1
dnl Copyright (C) 2007 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_MATH_H],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  gl_ABSOLUTE_HEADER([math.h])
  ABSOLUTE_MATH_H=\"$gl_cv_absolute_math_h\"
  AC_SUBST([ABSOLUTE_MATH_H])
])

AC_DEFUN([gl_MATH_MODULE_INDICATOR],
[
  dnl Use AC_REQUIRE here, so that the default settings are expanded once only.
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  GNULIB_[]m4_translit([$1],[abcdefghijklmnopqrstuvwxyz./-],[ABCDEFGHIJKLMNOPQRSTUVWXYZ___])=1
])

AC_DEFUN([gl_MATH_H_DEFAULTS],
[
  GNULIB_MATHL=0; AC_SUBST([GNULIB_MATHL])
])
