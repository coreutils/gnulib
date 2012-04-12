# sys_resource_h.m4 serial 1
dnl Copyright (C) 2012 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_HEADER_SYS_RESOURCE],
[
  AC_REQUIRE([gl_SYS_RESOURCE_H_DEFAULTS])
  dnl <sys/resource.h> is always overridden, because of GNULIB_POSIXCHECK.
  gl_CHECK_NEXT_HEADERS([sys/resource.h])
  if test $ac_cv_header_sys_resource_h = yes; then
    HAVE_SYS_RESOURCE_H=1
  else
    HAVE_SYS_RESOURCE_H=0
  fi
  AC_SUBST([HAVE_SYS_RESOURCE_H])
])

AC_DEFUN([gl_SYS_RESOURCE_MODULE_INDICATOR],
[
  dnl Use AC_REQUIRE here, so that the default settings are expanded once only.
  AC_REQUIRE([gl_SYS_RESOURCE_H_DEFAULTS])
  gl_MODULE_INDICATOR_SET_VARIABLE([$1])
  dnl Define it also as a C macro, for the benefit of the unit tests.
  gl_MODULE_INDICATOR_FOR_TESTS([$1])
])

AC_DEFUN([gl_SYS_RESOURCE_H_DEFAULTS],
[
])
