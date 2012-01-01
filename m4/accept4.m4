# accept4.m4 serial 2
dnl Copyright (C) 2009-2012 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_ACCEPT4],
[
  AC_REQUIRE([gl_SYS_SOCKET_H_DEFAULTS])

  dnl Persuade glibc <sys/socket.h> to declare accept4().
  AC_REQUIRE([AC_USE_SYSTEM_EXTENSIONS])

  AC_CHECK_FUNCS_ONCE([accept4])
  if test $ac_cv_func_accept4 != yes; then
    HAVE_ACCEPT4=0
  fi
])
