# unlink.m4 serial 1
dnl Copyright (C) 2009 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_UNLINK],
[
  AC_REQUIRE([gl_AC_DOS])
  AC_REQUIRE([gl_UNISTD_H_DEFAULTS])
  dnl Detect Solaris 9 bug.
  AC_CACHE_CHECK([whether unlink honors trailing slashes],
    [gl_cv_func_unlink_works],
    [touch conftest.file
     AC_RUN_IFELSE(
       [AC_LANG_PROGRAM(
         [[#include <stdio.h>
           #include <errno.h>
]], [[return !unlink ("conftest.file/") || errno != ENOTDIR;]])],
      [gl_cv_func_unlink_works=yes], [gl_cv_func_unlink_works=no],
      [gl_cv_func_unlink_works="guessing no"])
     rm -f conftest.file])
  if test x"$gl_cv_func_unlink_works" != xyes; then
    REPLACE_UNLINK=1
    AC_LIBOBJ([unlink])
  fi
])
