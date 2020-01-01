# uchar.m4 serial 1
dnl Copyright (C) 2019-2020 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl From Bruno Haible.
dnl Prepare the overridden <uchar.h>.

AC_DEFUN_ONCE([gl_UCHAR_H],
[
  gl_CHECK_NEXT_HEADERS([uchar.h])
  if test $ac_cv_header_uchar_h = yes; then
    HAVE_UCHAR_H=1
  else
    HAVE_UCHAR_H=0
  fi
  AC_SUBST([HAVE_UCHAR_H])
])
