# fchdir.m4 serial 9
dnl Copyright (C) 2006-2009 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_FCHDIR],
[
  AC_REQUIRE([gl_UNISTD_H_DEFAULTS])
  AC_REQUIRE([gl_DIRENT_H_DEFAULTS])
  AC_REQUIRE([gl_SYS_STAT_H_DEFAULTS])
  AC_CHECK_FUNCS_ONCE([fchdir])
  if test $ac_cv_func_fchdir = no; then
    REPLACE_FCHDIR=1
    AC_LIBOBJ([fchdir])
    gl_PREREQ_FCHDIR
    AC_DEFINE([REPLACE_FCHDIR], [1],
      [Define to 1 if gnulib's fchdir() replacement is used.])
    REPLACE_FSTAT=1
    gl_REPLACE_OPEN
    gl_REPLACE_CLOSE
    gl_REPLACE_DUP2
    dnl dup3 is already unconditionally replaced
    gl_REPLACE_DIRENT_H
    AC_CACHE_CHECK([whether open can visit directories],
      [gl_cv_func_open_directory_works],
      [AC_RUN_IFELSE([AC_LANG_PROGRAM([[#include <fcntl.h>
]], [return open(".", O_RDONLY);])],
        [gl_cv_func_open_directory_works=yes],
        [gl_cv_func_open_directory_works=no],
        [gl_cv_func_open_directory_works="guessing no"])])
    if test "$gl_cv_func_open_directory_works" != yes; then
      AC_DEFINE([REPLACE_OPEN_DIRECTORY], [1], [Define to 1 if open() should
work around the inability to open a directory.])
    fi
  fi
])

# Prerequisites of lib/fchdir.c.
AC_DEFUN([gl_PREREQ_FCHDIR], [:])
