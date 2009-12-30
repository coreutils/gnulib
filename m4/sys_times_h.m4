# Configure a replacement for <sys/times.h>.
# serial 2

# Copyright (C) 2008, 2009 Free Software Foundation, Inc.
# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# Written by Simon Josefsson.

AC_DEFUN([gl_SYS_TIMES_H],
[
  AC_REQUIRE([gl_SYS_TIMES_H_DEFAULTS])

  gl_CHECK_NEXT_HEADERS([sys/times.h])
  AC_CHECK_HEADERS_ONCE([sys/times.h])
  if test $ac_cv_header_sys_times_h = yes; then
    SYS_TIMES_H=
    HAVE_SYS_TIMES_H=1
    AC_CHECK_TYPES([struct tms], [], [HAVE_STRUCT_TMS=0], [[
#include <sys/times.h>
      ]])
  else
    SYS_TIMES_H=sys/times.h
    HAVE_SYS_TIMES_H=0
    HAVE_STRUCT_TMS=0
  fi
  AC_SUBST([SYS_TIMES_H])
  AC_SUBST([HAVE_SYS_TIMES_H])
])

AC_DEFUN([gl_SYS_TIMES_MODULE_INDICATOR],
[
  dnl Use AC_REQUIRE here, so that the default settings are expanded once only.
  AC_REQUIRE([gl_SYS_TIMES_H_DEFAULTS])
  GNULIB_[]m4_translit([$1],[abcdefghijklmnopqrstuvwxyz./-],[ABCDEFGHIJKLMNOPQRSTUVWXYZ___])=1
])

AC_DEFUN([gl_SYS_TIMES_H_DEFAULTS],
[
  GNULIB_TIMES=0;     AC_SUBST([GNULIB_TIMES])
  HAVE_STRUCT_TMS=1;  AC_SUBST([HAVE_STRUCT_TMS])
  HAVE_TIMES=1;       AC_SUBST([HAVE_TIMES])
])
