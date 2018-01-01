# truncate.m4 serial 1   -*- Autoconf -*-
dnl Copyright (C) 2017-2018 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_TRUNCATE],
[
  AC_REQUIRE([gl_UNISTD_H_DEFAULTS])
  AC_CHECK_FUNCS_ONCE([truncate])
  if test $ac_cv_func_truncate = yes; then
    m4_ifdef([gl_LARGEFILE], [
      AC_REQUIRE([AC_CANONICAL_HOST])
      case "$host_os" in
        mingw*)
          dnl Native Windows, and Large File Support is requested.
          dnl The mingw64 truncate64() function is based on ftruncate64(),
          dnl which is unreliable (it may delete the file, see
          dnl <http://mingw-w64.sourcearchive.com/documentation/2.0-1/ftruncate64_8c_source.html>).
          dnl Use gnulib's ftruncate() and truncate() implementation instead.
          REPLACE_TRUNCATE=1
          ;;
      esac
    ], [
      :
    ])
  else
    HAVE_TRUNCATE=0
  fi
])

# Prerequisites of lib/truncate.c.
AC_DEFUN([gl_PREREQ_TRUNCATE], [:])
