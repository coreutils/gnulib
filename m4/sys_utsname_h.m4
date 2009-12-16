# sys_utsname_h.m4 serial 4
dnl Copyright (C) 2009, 2010 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl Written by Bruno Haible.

AC_DEFUN([gl_SYS_UTSNAME_H],
[
  dnl Use AC_REQUIRE here, so that the default behavior below is expanded
  dnl once only, before all statements that occur in other macros.
  AC_REQUIRE([gl_SYS_UTSNAME_H_DEFAULTS])

  gl_CHECK_NEXT_HEADERS([sys/utsname.h])
  AC_CHECK_HEADERS_ONCE([sys/utsname.h])
  if test $ac_cv_header_sys_utsname_h != yes; then
    HAVE_SYS_UTSNAME_H=0
    HAVE_STRUCT_UTSNAME=0
  else
    HAVE_SYS_UTSNAME_H=1
    AC_CHECK_TYPES([struct utsname], [], [HAVE_STRUCT_UTSNAME=0], [[
#include <sys/utsname.h>
      ]])
  fi
  AC_SUBST([HAVE_SYS_UTSNAME_H])

  dnl Check for declarations of anything we want to poison if the
  dnl corresponding gnulib module is not in use.
  gl_WARN_ON_USE_PREPARE([[#include <sys/utsname.h>
    ]], [uname])
])

AC_DEFUN([gl_SYS_UTSNAME_MODULE_INDICATOR],
[
  dnl Use AC_REQUIRE here, so that the default settings are expanded once only.
  AC_REQUIRE([gl_SYS_UTSNAME_H_DEFAULTS])
  GNULIB_[]m4_translit([$1],[abcdefghijklmnopqrstuvwxyz./-],[ABCDEFGHIJKLMNOPQRSTUVWXYZ___])=1
])

AC_DEFUN([gl_SYS_UTSNAME_H_DEFAULTS],
[
  GNULIB_UNAME=0;         AC_SUBST([GNULIB_UNAME])
  dnl Assume proper GNU behavior unless another module says otherwise.
  HAVE_UNAME=1;           AC_SUBST([HAVE_UNAME])
  HAVE_STRUCT_UTSNAME=1;  AC_SUBST([HAVE_STRUCT_UTSNAME])
])
