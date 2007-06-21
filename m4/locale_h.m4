# locale_h.m4 serial 2
dnl Copyright (C) 2007 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_LOCALE_H],
[
  AC_CACHE_CHECK([whether locale.h conforms to POSIX],
    [gl_cv_header_working_locale_h],
    [AC_TRY_COMPILE([#include <locale.h>
int x = LC_MESSAGES;], [],
       [gl_cv_header_working_locale_h=yes],
       [gl_cv_header_working_locale_h=no])])
  if test $gl_cv_header_working_locale_h = yes; then
    LOCALE_H=
  else
    gl_CHECK_NEXT_HEADERS([locale.h])
    LOCALE_H=locale.h
  fi
  AC_SUBST([LOCALE_H])
])
