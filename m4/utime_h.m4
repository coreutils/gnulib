# utime_h.m4 serial 1
dnl Copyright (C) 2017 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl From Bruno Haible.

AC_DEFUN([gl_UTIME_H],
[
  UTIME_H=''
  AC_CHECK_HEADERS_ONCE([utime.h])
  if test $ac_cv_header_utime_h != yes; then
    dnl Provide a substitute <utime.h> file.
    UTIME_H=utime.h
  fi
  AC_SUBST([UTIME_H])
  AM_CONDITIONAL([GL_GENERATE_UTIME_H], [test -n "$UTIME_H"])
])
