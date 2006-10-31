# tsearch.m4 serial 1
dnl Copyright (C) 2006 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_TSEARCH],
[
  AC_REPLACE_FUNCS(tsearch)
  if test $ac_cv_func_tsearch = no; then
    gl_PREREQ_TSEARCH
  fi
])

# Prerequisites of lib/tsearch.c.
AC_DEFUN([gl_PREREQ_TSEARCH], [
  :
])
