# setlocale.m4 serial 5
dnl Copyright (C) 2011-2018 Free Software Foundation, Inc.
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
    dnl On Cygwin 1.5.x, setlocale always succeeds but setlocale(LC_CTYPE,NULL)
    dnl is then still "C".
    cygwin*)
      case `uname -r` in
        1.5.*) REPLACE_SETLOCALE=1 ;;
      esac
      ;;
  esac
])

# Prerequisites of lib/setlocale.c.
AC_DEFUN([gl_PREREQ_SETLOCALE],
[
  dnl No need to check for CFLocaleCopyPreferredLanguages and
  dnl CFPreferencesCopyAppValue because lib/setlocale.c is not used on Mac OS X.
  dnl (The Mac OS X specific code is only used in libintl.)
  :
])
