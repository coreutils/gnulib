# getsubopt.m4 serial 2
dnl Copyright (C) 2004 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_GETSUBOPT],
[
  dnl Persuade glibc <stdlib.h> to declare getsubopt().
  AC_REQUIRE([AC_GNU_SOURCE])

  AC_REPLACE_FUNCS(getsubopt)
  if test $ac_cv_func_getsubopt = no; then
    gl_PREREQ_GETSUBOPT
  fi
])

# Prerequisites of lib/getsubopt.c.
AC_DEFUN([gl_PREREQ_GETSUBOPT], [:])
