# mntent_h.m4
# serial 2
dnl Copyright (C) 2024-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN_ONCE([gl_MNTENT_H],
[
  dnl Ensure to expand the default settings once only, before all statements
  dnl that occur in other macros.
  AC_REQUIRE([gl_MNTENT_H_DEFAULTS])

  AC_CHECK_HEADERS_ONCE([mntent.h])
  dnl For now, we provide a <mntent.h> wrapper only if the system already has
  dnl a <mntent.h>.
  if test $ac_cv_header_mntent_h = yes; then
    GL_GENERATE_MNTENT_H=true
    gl_CHECK_NEXT_HEADERS([mntent.h])
  else
    GL_GENERATE_MNTENT_H=false
  fi

  dnl Check for declarations of anything we want to poison if the
  dnl corresponding gnulib module is not in use.
  gl_WARN_ON_USE_PREPARE([[#include <mntent.h>
    ]],
    [hasmntopt])
])

# gl_MNTENT_MODULE_INDICATOR([modulename])
# sets the shell variable that indicates the presence of the given module
# to a C preprocessor expression that will evaluate to 1.
# This macro invocation must not occur in macros that are AC_REQUIREd.
AC_DEFUN([gl_MNTENT_MODULE_INDICATOR],
[
  dnl Ensure to expand the default settings once only.
  gl_MNTENT_H_REQUIRE_DEFAULTS
  gl_MODULE_INDICATOR_SET_VARIABLE([$1])
  dnl Define it also as a C macro, for the benefit of the unit tests.
  gl_MODULE_INDICATOR_FOR_TESTS([$1])
])

# Initializes the default values for AC_SUBSTed shell variables.
# This macro must not be AC_REQUIREd.  It must only be invoked, and only
# outside of macros or in macros that are not AC_REQUIREd.
AC_DEFUN([gl_MNTENT_H_REQUIRE_DEFAULTS],
[
  m4_defun(GL_MODULE_INDICATOR_PREFIX[_MNTENT_H_MODULE_INDICATOR_DEFAULTS], [
    gl_MODULE_INDICATOR_INIT_VARIABLE([GNULIB_HASMNTOPT])
  ])
  m4_require(GL_MODULE_INDICATOR_PREFIX[_MNTENT_H_MODULE_INDICATOR_DEFAULTS])
  AC_REQUIRE([gl_MNTENT_H_DEFAULTS])
])

AC_DEFUN([gl_MNTENT_H_DEFAULTS],
[
  dnl Assume proper GNU behavior unless another module says otherwise.
  HAVE_HASMNTOPT=1;      AC_SUBST([HAVE_HASMNTOPT])
  HAVE_SETMNTENT=1;      AC_SUBST([HAVE_SETMNTENT])
  REPLACE_HASMNTOPT=0;   AC_SUBST([REPLACE_HASMNTOPT])
])
