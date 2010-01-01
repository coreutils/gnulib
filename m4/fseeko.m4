# fseeko.m4 serial 7
dnl Copyright (C) 2007-2010 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_FSEEKO],
[
  AC_REQUIRE([gl_STDIO_H_DEFAULTS])
  AC_REQUIRE([AC_PROG_CC])
  AC_REQUIRE([gl_STDIN_LARGE_OFFSET])

  dnl Persuade glibc <stdio.h> to declare fseeko().
  AC_REQUIRE([AC_USE_SYSTEM_EXTENSIONS])

  AC_CACHE_CHECK([for fseeko], [gl_cv_func_fseeko],
    [
      AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <stdio.h>
]], [fseeko (stdin, 0, 0);])],
        [gl_cv_func_fseeko=yes], [gl_cv_func_fseeko=no])
    ])
  if test $gl_cv_func_fseeko = no \
      || test $gl_cv_var_stdin_large_offset = no; then
    gl_REPLACE_FSEEKO
  fi
])

AC_DEFUN([gl_REPLACE_FSEEKO],
[
  AC_LIBOBJ([fseeko])
  AC_REQUIRE([gl_STDIO_H_DEFAULTS])
  REPLACE_FSEEKO=1
  dnl If we are also using the fseek module, then fseek needs replacing, too.
  m4_ifdef([gl_REPLACE_FSEEK], [gl_REPLACE_FSEEK])
])
