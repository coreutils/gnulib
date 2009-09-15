# serial 1

# Copyright (C) 2009 Free Software Foundation, Inc.
#
# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_STAT],
[
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  AC_REQUIRE([gl_AC_DOS])
  AC_REQUIRE([gl_SYS_STAT_H_DEFAULTS])
  dnl mingw is the only known platform where stat(".") and stat("./") differ
  AC_CACHE_CHECK([whether stat handles trailing slashes],
      [gl_cv_func_stat_works],
      [AC_RUN_IFELSE(
         [AC_LANG_PROGRAM(
           [[#include <sys/stat.h>
]], [[struct stat st; return stat (".", &st) != stat ("./", &st);]])],
         [gl_cv_func_stat_works=yes], [gl_cv_func_stat_works=no],
         [case $host_os in
            mingw*) gl_cv_func_stat_works="guessing no";;
            *) gl_cv_func_stat_works="guessing yes";;
          esac])])
  case $gl_cv_func_stat_works in
    *yes) ;;
    *) REPLACE_STAT=1
       AC_LIBOBJ([stat]);;
  esac
])
