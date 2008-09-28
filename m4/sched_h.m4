# sched_h.m4 serial 1
dnl Copyright (C) 2008 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl Written by Bruno Haible.

AC_DEFUN([gl_SCHED_H],
[
  AC_CHECK_HEADERS_ONCE([sched.h])
  if test $ac_cv_header_sched_h = yes; then
    SCHED_H=''
  else
    SCHED_H='sched.h'
  fi
  AC_SUBST([SCHED_H])
])
