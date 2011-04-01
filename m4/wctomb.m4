# wctomb.m4 serial 1
dnl Copyright (C) 2011 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_WCTOMB],
[
  AC_REQUIRE([gl_STDLIB_H_DEFAULTS])

  if false; then
    REPLACE_WCTOMB=1
  fi
  if test $REPLACE_WCTOMB = 1; then
    AC_LIBOBJ([wctomb])
    gl_PREREQ_WCTOMB
  fi
])

# Prerequisites of lib/wctomb.c.
AC_DEFUN([gl_PREREQ_WCTOMB], [
  :
])
