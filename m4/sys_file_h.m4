# Configure a replacement for <sys/file.h>.
# serial 8

# Copyright (C) 2008-2021 Free Software Foundation, Inc.
# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# Written by Richard W.M. Jones.

AC_DEFUN_ONCE([gl_SYS_FILE_H],
[
  AC_REQUIRE([gl_SYS_FILE_H_DEFAULTS])

  dnl <sys/file.h> is always overridden, because of GNULIB_POSIXCHECK.
  gl_CHECK_NEXT_HEADERS([sys/file.h])

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

AC_DEFUN([gl_SYS_FILE_MODULE_INDICATOR],
[
  gl_SYS_FILE_H_REQUIRE_DEFAULTS
  gl_MODULE_INDICATOR_SET_VARIABLE([$1])
])

AC_DEFUN([gl_SYS_FILE_H_REQUIRE_DEFAULTS],
[
  m4_defun(GL_MODULE_INDICATOR_PREFIX[_SYS_FILE_H_MODULE_INDICATOR_DEFAULTS], [
    gl_MODULE_INDICATOR_INIT_VARIABLE([GNULIB_FLOCK])
  ])
  m4_require(GL_MODULE_INDICATOR_PREFIX[_SYS_FILE_H_MODULE_INDICATOR_DEFAULTS])
  AC_REQUIRE([gl_SYS_FILE_H_DEFAULTS])
])

AC_DEFUN([gl_SYS_FILE_H_DEFAULTS],
[
  HAVE_FLOCK=1;          AC_SUBST([HAVE_FLOCK])
])
