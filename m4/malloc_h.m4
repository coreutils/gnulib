# malloc_h.m4 serial 4
dnl Copyright (C) 2020-2022 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl Written by Bruno Haible.

AC_DEFUN_ONCE([gl_MALLOC_H],
[
  dnl Ensure to expand the default settings once only, before all statements
  dnl that occur in other macros.
  AC_REQUIRE([gl_MALLOC_H_DEFAULTS])

  gl_CHECK_NEXT_HEADERS([malloc.h])
  if test $ac_cv_header_malloc_h = yes; then
    HAVE_MALLOC_H=1
  else
    HAVE_MALLOC_H=0
  fi
  AC_SUBST([HAVE_MALLOC_H])

  dnl Check for declarations of anything we want to poison if the
  dnl corresponding gnulib module is not in use.
  gl_WARN_ON_USE_PREPARE([[
#if HAVE_MALLOC_H
# include <malloc.h>
#endif
    ]], [memalign])
])

# gl_MALLOC_MODULE_INDICATOR([modulename])
# sets the shell variable that indicates the presence of the given module
# to a C preprocessor expression that will evaluate to 1.
# This macro invocation must not occur in macros that are AC_REQUIREd.
AC_DEFUN([gl_MALLOC_MODULE_INDICATOR],
[
  dnl Ensure to expand the default settings once only.
  gl_MALLOC_H_REQUIRE_DEFAULTS
  gl_MODULE_INDICATOR_SET_VARIABLE([$1])
  dnl Define it also as a C macro, for the benefit of the unit tests.
  gl_MODULE_INDICATOR_FOR_TESTS([$1])
])

# Initializes the default values for AC_SUBSTed shell variables.
# This macro must not be AC_REQUIREd.  It must only be invoked, and only
# outside of macros or in macros that are not AC_REQUIREd.
AC_DEFUN([gl_MALLOC_H_REQUIRE_DEFAULTS],
[
  m4_defun(GL_MODULE_INDICATOR_PREFIX[_MALLOC_H_MODULE_INDICATOR_DEFAULTS], [
    gl_MODULE_INDICATOR_INIT_VARIABLE([GNULIB_MEMALIGN])
  ])
  m4_require(GL_MODULE_INDICATOR_PREFIX[_MALLOC_H_MODULE_INDICATOR_DEFAULTS])
  AC_REQUIRE([gl_MALLOC_H_DEFAULTS])
])

AC_DEFUN([gl_MALLOC_H_DEFAULTS],
[
  dnl Assume proper GNU behavior unless another module says otherwise.
  HAVE_MEMALIGN=1;        AC_SUBST([HAVE_MEMALIGN])
  REPLACE_MEMALIGN=0;     AC_SUBST([REPLACE_MEMALIGN])
])
