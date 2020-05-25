# getrandom.m4
dnl Copyright 2020 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl Written by Paul Eggert.

AC_DEFUN([gl_FUNC_GETRANDOM],
[
  AC_CHECK_FUNCS_ONCE([getrandom])
  if test "$ac_cv_func_getrandom" != yes; then
    HAVE_GETRANDOM=0
  fi
])
