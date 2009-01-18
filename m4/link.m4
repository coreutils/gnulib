# link.m4 serial 1
dnl Copyright (C) 2009 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_LINK],
[
  AC_REQUIRE([gl_UNISTD_H_DEFAULTS])
  AC_CHECK_FUNCS_ONCE([link])
  if test $ac_cv_func_link = no; then
    HAVE_LINK=0
    AC_LIBOBJ([link])
    gl_PREREQ_LINK
  fi
])

# Prerequisites of lib/link.c.
AC_DEFUN([gl_PREREQ_LINK], [:])
