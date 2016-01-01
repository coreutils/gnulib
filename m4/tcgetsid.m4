# tcgetsid.m4 serial 4
dnl Copyright (C) 2010-2016 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_TCGETSID],
[
  AC_REQUIRE([gl_TERMIOS_H_DEFAULTS])

  dnl Persuade glibc <termios.h> to declare tcgetsid().
  AC_REQUIRE([AC_USE_SYSTEM_EXTENSIONS])

  AC_CHECK_DECLS([tcgetsid], , , [[#include <termios.h>]])
  if test $ac_cv_have_decl_tcgetsid = no; then
    HAVE_DECL_TCGETSID=0
  fi

  AC_CHECK_FUNCS([tcgetsid])
  if test $ac_cv_func_tcgetsid = yes; then
    HAVE_TCGETSID=1
  else
    HAVE_TCGETSID=0
  fi
])

# Prerequisites of lib/tcgetsid.c.
AC_DEFUN([gl_PREREQ_TCGETSID], [
  :
])
