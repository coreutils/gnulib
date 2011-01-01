# ftell.m4 serial 1
dnl Copyright (C) 2007, 2009-2011 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_FTELL],
[
  AC_REQUIRE([gl_STDIO_H_DEFAULTS])
  AC_REQUIRE([gl_FUNC_FTELLO])
  dnl When ftello needs fixes, ftell needs them too.
  if test $REPLACE_FTELLO != 0; then
    AC_LIBOBJ([ftell])
    REPLACE_FTELL=1
  fi
])
