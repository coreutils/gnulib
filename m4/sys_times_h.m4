# Configure a replacement for <sys/times.h>.

# Copyright (C) 2008, 2009 Free Software Foundation, Inc.
# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# Written by Simon Josefsson.

AC_DEFUN([gl_SYS_TIMES_H],
[
  AC_REQUIRE([gl_SYS_TIMES_H_DEFAULTS])

  AC_CHECK_HEADERS_ONCE([sys/times.h])
  if test $ac_cv_header_sys_times_h = yes; then
    SYS_TIMES_H=
  else
    SYS_TIMES_H=sys/times.h
  fi
  AC_SUBST([SYS_TIMES_H])
])

AC_DEFUN([gl_SYS_TIMES_MODULE_INDICATOR],
[
  dnl Use AC_REQUIRE here, so that the default settings are expanded once only.
  AC_REQUIRE([gl_SYS_TIMES_H_DEFAULTS])
  GNULIB_[]m4_translit([$1],[abcdefghijklmnopqrstuvwxyz./-],[ABCDEFGHIJKLMNOPQRSTUVWXYZ___])=1
])

AC_DEFUN([gl_SYS_TIMES_H_DEFAULTS],
[
  GNULIB_TIMES=0; AC_SUBST([GNULIB_TIMES])
])
