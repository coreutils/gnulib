# dirent_h.m4 serial 2
dnl Copyright (C) 2008 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl Written by Bruno Haible.

AC_DEFUN([gl_DIRENT_H],
[
  dnl Use AC_REQUIRE here, so that the default behavior below is expanded
  dnl once only, before all statements that occur in other macros.
  AC_REQUIRE([gl_DIRENT_H_DEFAULTS])

  gl_CHECK_NEXT_HEADERS([dirent.h])
])

dnl Unconditionally enables the replacement of <dirent.h>.
AC_DEFUN([gl_REPLACE_DIRENT_H],
[
  AC_REQUIRE([gl_DIRENT_H_DEFAULTS])
  DIRENT_H='dirent.h'
])

AC_DEFUN([gl_DIRENT_MODULE_INDICATOR],
[
  dnl Use AC_REQUIRE here, so that the default settings are expanded once only.
  AC_REQUIRE([gl_DIRENT_H_DEFAULTS])
  GNULIB_[]m4_translit([$1],[abcdefghijklmnopqrstuvwxyz./-],[ABCDEFGHIJKLMNOPQRSTUVWXYZ___])=1
])

AC_DEFUN([gl_DIRENT_H_DEFAULTS],
[
  AC_REQUIRE([gl_UNISTD_H_DEFAULTS]) dnl for REPLACE_FCHDIR
  GNULIB_DIRFD=0;    AC_SUBST([GNULIB_DIRFD])
  dnl Assume proper GNU behavior unless another module says otherwise.
  HAVE_DECL_DIRFD=1; AC_SUBST([HAVE_DECL_DIRFD])
  DIRENT_H='';       AC_SUBST([DIRENT_H])
])
