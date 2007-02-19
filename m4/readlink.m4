# readlink.m4 serial 3
dnl Copyright (C) 2003, 2007 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_READLINK],
[
  AC_REQUIRE([gl_UNISTD_H_DEFAULTS])
  AC_CHECK_FUNCS(readlink)
  if test $ac_cv_func_readlink = no; then
    HAVE_READLINK=0
    AC_LIBOBJ(readlink)
    gl_PREREQ_READLINK
  fi
])

# Prerequisites of lib/readlink.c.
AC_DEFUN([gl_PREREQ_READLINK],
[
  :
])
