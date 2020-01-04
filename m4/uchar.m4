# uchar.m4 serial 3
dnl Copyright (C) 2019-2020 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl From Bruno Haible.
dnl Prepare the overridden <uchar.h>.

AC_DEFUN_ONCE([gl_UCHAR_H],
[
  AC_REQUIRE([gl_UCHAR_H_DEFAULTS])

  gl_CHECK_NEXT_HEADERS([uchar.h])
  if test $ac_cv_header_uchar_h = yes; then
    HAVE_UCHAR_H=1
  else
    HAVE_UCHAR_H=0
  fi
  AC_SUBST([HAVE_UCHAR_H])

  dnl Check for declarations of anything we want to poison if the
  dnl corresponding gnulib module is not in use, and which is not
  dnl guaranteed by C11.
  gl_WARN_ON_USE_PREPARE([[#include <uchar.h>
    ]], [mbrtoc32])
])

AC_DEFUN([gl_UCHAR_MODULE_INDICATOR],
[
  dnl Use AC_REQUIRE here, so that the default settings are expanded once only.
  AC_REQUIRE([gl_UCHAR_H_DEFAULTS])
  gl_MODULE_INDICATOR_SET_VARIABLE([$1])
  dnl Define it also as a C macro, for the benefit of the unit tests.
  gl_MODULE_INDICATOR_FOR_TESTS([$1])
])

AC_DEFUN([gl_UCHAR_H_DEFAULTS],
[
  GNULIB_C32TOB=0;           AC_SUBST([GNULIB_C32TOB])
  GNULIB_MBRTOC32=0;         AC_SUBST([GNULIB_MBRTOC32])
  dnl Assume proper GNU behavior unless another module says otherwise.
  HAVE_MBRTOC32=1;           AC_SUBST([HAVE_MBRTOC32])
  REPLACE_MBRTOC32=0;        AC_SUBST([REPLACE_MBRTOC32])
])
