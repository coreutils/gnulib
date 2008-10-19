# getusershell.m4 serial 5
dnl Copyright (C) 2002, 2003, 2006, 2008 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_GETUSERSHELL],
[
  AC_REQUIRE([gl_UNISTD_H_DEFAULTS])

  dnl Persuade glibc <unistd.h> to declare {get,set,end}usershell().
  AC_REQUIRE([AC_USE_SYSTEM_EXTENSIONS])

  AC_REPLACE_FUNCS([getusershell])
  if test $ac_cv_func_getusershell = no; then
    HAVE_GETUSERSHELL=0
  fi
])
