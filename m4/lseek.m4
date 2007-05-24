# lseek.m4 serial 1
dnl Copyright (C) 2007 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_LSEEK],
[
  AC_REQUIRE([gl_UNISTD_H_DEFAULTS])
  AC_REQUIRE([AC_PROG_CC])
  AC_CACHE_CHECK([whether lseek detects pipes], [gl_cv_func_lseek_pipe],
    [AC_COMPILE_IFELSE([AC_LANG_PROGRAM([#include <unistd.h>],
[#if (defined _WIN32 || defined __WIN32__) && ! defined __CYGWIN__
/* mingw mistakenly returns 0 when trying to seek on pipes.  */
  Choke me.
#endif])],
      [gl_cv_func_lseek_pipe=yes], [gl_cv_func_lseek_pipe=no])])
  if test $gl_cv_func_lseek_pipe = no; then
    gl_REPLACE_LSEEK
  fi
])

AC_DEFUN([gl_REPLACE_LSEEK],
[
  AC_LIBOBJ([lseek])
  AC_REQUIRE([gl_UNISTD_H_DEFAULTS])
  REPLACE_LSEEK=1
  AC_DEFINE([LSEEK_PIPE_BROKEN], [1],
	    [Define to 1 if lseek does not detect pipes.])
])
