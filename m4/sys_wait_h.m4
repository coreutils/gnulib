# sys_wait_h.m4 serial 2
dnl Copyright (C) 2008, 2009, 2010 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_SYS_WAIT_H],
[
  AC_REQUIRE([gl_SYS_WAIT_H_DEFAULTS])

  gl_CHECK_NEXT_HEADERS([sys/wait.h])
])

AC_DEFUN([gl_SYS_WAIT_MODULE_INDICATOR],
[
  dnl Use AC_REQUIRE here, so that the default settings are expanded once only.
  AC_REQUIRE([gl_SYS_WAIT_H_DEFAULTS])
  GNULIB_[]m4_translit([$1],[abcdefghijklmnopqrstuvwxyz./-],[ABCDEFGHIJKLMNOPQRSTUVWXYZ___])=1
])

AC_DEFUN([gl_SYS_WAIT_H_DEFAULTS],
[
  dnl Assume proper GNU behavior unless another module says otherwise.
])
