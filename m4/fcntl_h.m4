# serial 9
# Configure fcntl.h.
dnl Copyright (C) 2006, 2007, 2009, 2010 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl Written by Paul Eggert.

AC_DEFUN([gl_FCNTL_H],
[
  AC_REQUIRE([gl_FCNTL_H_DEFAULTS])
  AC_REQUIRE([gl_FCNTL_O_FLAGS])
  gl_CHECK_NEXT_HEADERS([fcntl.h])
])

AC_DEFUN([gl_FCNTL_MODULE_INDICATOR],
[
  dnl Use AC_REQUIRE here, so that the default settings are expanded once only.
  AC_REQUIRE([gl_FCNTL_H_DEFAULTS])
  GNULIB_[]m4_translit([$1],[abcdefghijklmnopqrstuvwxyz./-],[ABCDEFGHIJKLMNOPQRSTUVWXYZ___])=1
])

AC_DEFUN([gl_FCNTL_H_DEFAULTS],
[
  GNULIB_FCNTL=0;   AC_SUBST([GNULIB_FCNTL])
  GNULIB_OPEN=0;    AC_SUBST([GNULIB_OPEN])
  GNULIB_OPENAT=0;  AC_SUBST([GNULIB_OPENAT])
  dnl Assume proper GNU behavior unless another module says otherwise.
  HAVE_FCNTL=1;     AC_SUBST([HAVE_FCNTL])
  HAVE_OPENAT=1;    AC_SUBST([HAVE_OPENAT])
  REPLACE_FCNTL=0;  AC_SUBST([REPLACE_FCNTL])
  REPLACE_OPEN=0;   AC_SUBST([REPLACE_OPEN])
  REPLACE_OPENAT=0; AC_SUBST([REPLACE_OPENAT])
])
