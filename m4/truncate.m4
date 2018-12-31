# truncate.m4 serial 2   -*- Autoconf -*-
dnl Copyright (C) 2017-2019 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_TRUNCATE],
[
  AC_REQUIRE([gl_UNISTD_H_DEFAULTS])
  AC_CHECK_FUNCS_ONCE([truncate])
  dnl AC_CHECK_FUNC is not enough here, because when compiling for Android 4.4
  dnl or older with _FILE_OFFSET_BITS=64, truncate() is not declared.  There
  dnl is a function 'truncate' in libc, but it is unsuitable, because it takes
  dnl only a 32-bit offset argument.
  AC_CHECK_DECL([truncate], , , [[#include <unistd.h>]])
  if test $ac_cv_have_decl_truncate = yes; then
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
    HAVE_DECL_TRUNCATE=0
    if test $ac_cv_func_truncate = yes; then
      dnl Avoid a conflict with the 'truncate' in libc.
      REPLACE_TRUNCATE=1
    fi
  fi
])

# Prerequisites of lib/truncate.c.
AC_DEFUN([gl_PREREQ_TRUNCATE], [:])
