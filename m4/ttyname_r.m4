# ttyname_r.m4 serial 2
dnl Copyright (C) 2010 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_TTYNAME_R],
[
  AC_REQUIRE([gl_UNISTD_H_DEFAULTS])

  AC_CHECK_FUNCS([ttyname_r])
  if test $ac_cv_func_ttyname_r = no; then
    HAVE_TTYNAME_R=0
  else
    dnl On MacOS X 10.4 and Solaris 10 the return type is 'char *', not 'int'.
    AC_CACHE_CHECK([whether ttyname_r is compatible with its POSIX signature],
      [gl_cv_func_ttyname_r_posix],
      [AC_COMPILE_IFELSE(
         [AC_LANG_PROGRAM(
            [[#include <stddef.h>
              #include <unistd.h>]],
            [[*ttyname_r (0, NULL, 0);]])
         ],
         [gl_cv_func_ttyname_r_posix=no],
         [gl_cv_func_ttyname_r_posix=yes])
      ])
    if test $gl_cv_func_ttyname_r_posix = no; then
      REPLACE_TTYNAME_R=1
    fi
  fi
  if test $HAVE_TTYNAME_R = 0 || test $REPLACE_TTYNAME_R = 1; then
    AC_LIBOBJ([ttyname_r])
    gl_PREREQ_TTYNAME_R
  fi
])

# Prerequisites of lib/ttyname_r.c.
AC_DEFUN([gl_PREREQ_TTYNAME_R], [
  AC_CHECK_FUNCS([ttyname])
])
