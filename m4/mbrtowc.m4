# mbrtowc.m4 serial 10
dnl Copyright (C) 2001-2002, 2004-2005, 2008 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_MBRTOWC],
[
  AC_REQUIRE([gl_WCHAR_H_DEFAULTS])

  AC_REQUIRE([AC_TYPE_MBSTATE_T])
  AC_CHECK_FUNCS_ONCE([mbrtowc])
  if test $ac_cv_func_mbrtowc = no; then
    HAVE_MBRTOWC=0
    gl_REPLACE_WCHAR_H
    AC_LIBOBJ([mbrtowc])
    gl_PREREQ_MBRTOWC
  fi
])

# Prerequisites of lib/mbrtowc.c.
AC_DEFUN([gl_PREREQ_MBRTOWC], [
  :
])


dnl From Paul Eggert

dnl This override of an autoconf macro can be removed when autoconf 2.60 or
dnl newer can be assumed everywhere.

m4_if(m4_version_compare(m4_defn([m4_PACKAGE_VERSION]),[2.60]),[-1],[
AC_DEFUN([AC_FUNC_MBRTOWC],
[
  dnl Same as AC_FUNC_MBRTOWC in autoconf-2.60.
  AC_CACHE_CHECK([whether mbrtowc and mbstate_t are properly declared],
    gl_cv_func_mbrtowc,
    [AC_LINK_IFELSE(
       [AC_LANG_PROGRAM(
            [[#include <wchar.h>]],
            [[wchar_t wc;
              char const s[] = "";
              size_t n = 1;
              mbstate_t state;
              return ! (sizeof state && (mbrtowc) (&wc, s, n, &state));]])],
       gl_cv_func_mbrtowc=yes,
       gl_cv_func_mbrtowc=no)])
  if test $gl_cv_func_mbrtowc = yes; then
    AC_DEFINE([HAVE_MBRTOWC], 1,
      [Define to 1 if mbrtowc and mbstate_t are properly declared.])
  fi
])
])
