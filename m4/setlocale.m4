# setlocale.m4 serial 2
dnl Copyright (C) 2011 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_SETLOCALE],
[
  AC_REQUIRE([gl_LOCALE_H_DEFAULTS])
  AC_REQUIRE([AC_CANONICAL_HOST])
  case "$host_os" in
    dnl On native Windows systems, setlocale(category,NULL) does not look at
    dnl the environment variables LC_ALL, category, and LANG.
    mingw*) REPLACE_SETLOCALE=1 ;;
  esac
  if test $REPLACE_SETLOCALE = 1; then
    AC_LIBOBJ([setlocale])
    gl_PREREQ_SETLOCALE
  fi
])

# Prerequisites of lib/setlocale.c.
AC_DEFUN([gl_PREREQ_SETLOCALE],
[
  :
])
