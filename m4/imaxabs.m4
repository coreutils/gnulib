# imaxabs.m4 serial 1
dnl Copyright (C) 2006, 2009, 2010 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_IMAXABS],
[
  AC_REQUIRE([gl_INTTYPES_H])
  if test "$ac_cv_have_decl_imaxabs" != yes; then
    AC_LIBOBJ([imaxabs])
    gl_PREREQ_IMAXABS
  fi
])

# Prerequisites of lib/imaxabs.c.
AC_DEFUN([gl_PREREQ_IMAXABS], [:])
