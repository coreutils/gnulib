# gethostname.m4 serial 6
dnl Copyright (C) 2002, 2008, 2009 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_GETHOSTNAME],
[
  AC_REQUIRE([gl_UNISTD_H_DEFAULTS])
  gl_PREREQ_SYS_H_WINSOCK2

  dnl Where is gethostname() defined?
  dnl - On native Windows, it is in ws2_32.dll.
  dnl - Otherwise is is in libc.
  GETHOSTNAME_LIB=
  AC_CHECK_FUNCS([gethostname], , [
    AC_CACHE_CHECK([for gethostname in winsock2.h and -lws2_32],
      [gl_cv_w32_gethostname],
      [gl_cv_w32_gethostname=no
       gl_save_LIBS="$LIBS"
       LIBS="$LIBS -lws2_32"
       AC_TRY_LINK([
#ifdef HAVE_WINSOCK2_H
#include <winsock2.h>
#endif
#include <stddef.h>
], [gethostname(NULL, 0);], [gl_cv_w32_gethostname=yes])
       LIBS="$gl_save_LIBS"
      ])
    if test "$gl_cv_w32_gethostname" = "yes"; then
      GETHOSTNAME_LIB="-lws2_32"
    fi
  ])
  AC_SUBST([GETHOSTNAME_LIB])

  if test "$ac_cv_func_gethostname" = no; then
    AC_LIBOBJ([gethostname])
    HAVE_GETHOSTNAME=0
    gl_PREREQ_GETHOSTNAME
  fi
])

# Prerequisites of lib/gethostname.c.
AC_DEFUN([gl_PREREQ_GETHOSTNAME], [
  if test "$gl_cv_w32_gethostname" != "yes"; then
    AC_CHECK_FUNCS([uname])
  fi
])
