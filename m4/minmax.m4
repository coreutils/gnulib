# minmax.m4 serial 1
dnl Copyright (C) 2005 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_MINMAX],
[
  AC_REQUIRE([gl_PREREQ_MINMAX])
])

# Prerequisites of lib/minmax.h.
AC_DEFUN([gl_PREREQ_MINMAX],
[
  gl_MINMAX_IN_HEADER([limits.h])
  gl_MINMAX_IN_HEADER([sys/param.h])
])

dnl gl_MINMAX_IN_HEADER(HEADER)
AC_DEFUN([gl_MINMAX_IN_HEADER],
[
  define([header],[translit([$1],[./-],
                                 [___])])
  define([HEADER],[translit([$1],[abcdefghijklmnopqrstuvwxyz./-],
                                 [ABCDEFGHIJKLMNOPQRSTUVWXYZ___])])
  AC_CACHE_CHECK([whether <$1> defines MIN and MAX],
    [gl_cv_minmax_in_]header,
    [AC_TRY_COMPILE([#include <$1>
int x = MIN (42, 17);], [],
       [gl_cv_minmax_in_]header[=yes],
       [gl_cv_minmax_in_]header[=no])])
  if test $gl_cv_minmax_in_[]header = yes; then
    AC_DEFINE([HAVE_MINMAX_IN_]HEADER, 1,
      [Define to 1 if <$1> defines the MIN and MAX macros.])
  fi
  undefine([HEADER])
  undefine([header])
])
