# Configure a replacement for <sys/file.h>.
# serial 3

# Copyright (C) 2008, 2009, 2010 Free Software Foundation, Inc.
# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# Written by Richard W.M. Jones.

AC_DEFUN([gl_HEADER_SYS_FILE_H],
[
  AC_REQUIRE([gl_HEADER_SYS_FILE_H_DEFAULTS])

  dnl Only flock is defined in a working <sys/file.h>.  If that
  dnl function is already there, we don't want to do any substitution.
  AC_CHECK_FUNCS_ONCE([flock])

  gl_CHECK_NEXT_HEADERS([sys/file.h])

  AC_CHECK_HEADERS_ONCE([sys/file.h])
  if test $ac_cv_header_sys_file_h = yes; then
    HAVE_SYS_FILE_H=1
  else
    HAVE_SYS_FILE_H=0
  fi
  AC_SUBST([HAVE_SYS_FILE_H])

  dnl Check for declarations of anything we want to poison if the
  dnl corresponding gnulib module is not in use.
  gl_WARN_ON_USE_PREPARE([[#include <sys/file.h>
    ]], [flock])
])

AC_DEFUN([gl_HEADER_SYS_FILE_MODULE_INDICATOR],
[
  AC_REQUIRE([gl_HEADER_SYS_FILE_H_DEFAULTS])
  GNULIB_[]m4_translit([$1],[abcdefghijklmnopqrstuvwxyz./-],[ABCDEFGHIJKLMNOPQRSTUVWXYZ___])=1
])

AC_DEFUN([gl_HEADER_SYS_FILE_H_DEFAULTS],
[
  GNULIB_FLOCK=0;        AC_SUBST([GNULIB_FLOCK])
  HAVE_FLOCK=1;          AC_SUBST([HAVE_FLOCK])
])
