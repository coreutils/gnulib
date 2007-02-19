# stdlib_h.m4 serial 1
dnl Copyright (C) 2007 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_STDLIB_H],
[
  AC_REQUIRE([gl_STDLIB_H_DEFAULTS])
  gl_ABSOLUTE_HEADER([stdlib.h])
  ABSOLUTE_STDLIB_H=\"$gl_cv_absolute_stdlib_h\"
  AC_SUBST([ABSOLUTE_STDLIB_H])
])

AC_DEFUN([gl_STDLIB_MODULE_INDICATOR],
[
  dnl Use AC_REQUIRE here, so that the default settings are expanded once only.
  AC_REQUIRE([gl_STDLIB_H_DEFAULTS])
  GNULIB_[]m4_translit([$1],[abcdefghijklmnopqrstuvwxyz./-],[ABCDEFGHIJKLMNOPQRSTUVWXYZ___])=1
])

AC_DEFUN([gl_STDLIB_H_DEFAULTS],
[
  GNULIB_GETSUBOPT=0; AC_SUBST([GNULIB_GETSUBOPT])
  GNULIB_MKDTEMP=0;   AC_SUBST([GNULIB_MKDTEMP])
  GNULIB_MKSTEMP=0;   AC_SUBST([GNULIB_MKSTEMP])
  dnl Assume proper GNU behavior unless another module says otherwise.
  HAVE_GETSUBOPT=1;   AC_SUBST([HAVE_GETSUBOPT])
  HAVE_MKDTEMP=1;     AC_SUBST([HAVE_MKDTEMP])
  REPLACE_MKSTEMP=0;  AC_SUBST([REPLACE_MKSTEMP])
])
