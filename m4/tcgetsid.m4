# tcgetsid.m4 serial 1
dnl Copyright (C) 2010 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_TCGETSID],
[
  AC_REQUIRE([gl_TERMIOS_H_DEFAULTS])

  dnl Persuade glibc <termios.h> to declare tcgetsid().
  AC_REQUIRE([AC_USE_SYSTEM_EXTENSIONS])

  AC_CHECK_FUNCS([tcgetsid])
  if test $ac_cv_func_tcgetsid = no; then
    HAVE_TCGETSID=0
    AC_LIBOBJ([tcgetsid])
    gl_PREREQ_TCGETSID
  fi
])

# Prerequisites of lib/tcgetsid.c.
AC_DEFUN([gl_PREREQ_TCGETSID], [
  :
])
