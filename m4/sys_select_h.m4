# sys_select_h.m4 serial 1
dnl Copyright (C) 2006 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl Adapted from arpa_inet.m4, written by Simon Josefsson.

AC_DEFUN([gl_HEADER_SYS_SELECT],
[
  AC_CHECK_HEADERS_ONCE([sys/select.h])
  if test $ac_cv_header_sys_select_h = yes; then
    SYS_SELECT_H=''
  else
    SYS_SELECT_H='sys/select.h'
  fi
  AC_SUBST(SYS_SELECT_H)
])
