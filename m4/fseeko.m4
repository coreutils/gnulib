# fseeko.m4 serial 9
dnl Copyright (C) 2007-2010 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_FSEEKO],
[
  AC_REQUIRE([gl_STDIO_H_DEFAULTS])
  AC_REQUIRE([gl_HAVE_FSEEKO])
  AC_REQUIRE([gl_STDIN_LARGE_OFFSET])

  if test $gl_cv_func_fseeko = no; then
    HAVE_FSEEKO=0
    gl_REPLACE_FSEEKO
  else
    if test $gl_cv_var_stdin_large_offset = no; then
      gl_REPLACE_FSEEKO
    fi
  fi
])

dnl Tests whether fseeko is available.
dnl Result is gl_cv_func_fseeko.
AC_DEFUN([gl_HAVE_FSEEKO],
[
  AC_REQUIRE([AC_PROG_CC])

  dnl Persuade glibc <stdio.h> to declare fseeko().
  AC_REQUIRE([AC_USE_SYSTEM_EXTENSIONS])

  AC_CACHE_CHECK([for fseeko], [gl_cv_func_fseeko],
    [
      AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <stdio.h>
]], [fseeko (stdin, 0, 0);])],
        [gl_cv_func_fseeko=yes], [gl_cv_func_fseeko=no])
    ])
])

AC_DEFUN([gl_REPLACE_FSEEKO],
[
  AC_REQUIRE([gl_STDIO_H_DEFAULTS])
  AC_REQUIRE([gl_HAVE_FSEEKO])
  if test $gl_cv_func_fseeko = yes; then
    REPLACE_FSEEKO=1
  fi
  AC_LIBOBJ([fseeko])
  dnl If we are also using the fseek module, then fseek needs replacing, too.
  m4_ifdef([gl_REPLACE_FSEEK], [gl_REPLACE_FSEEK])
])
