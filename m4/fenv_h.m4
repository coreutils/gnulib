# fenv_h.m4
# serial 2
dnl Copyright (C) 2023-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN_ONCE([gl_FENV_H],
[
  dnl Ensure to expand the default settings once only, before all statements
  dnl that occur in other macros.
  AC_REQUIRE([gl_FENV_H_DEFAULTS])

  gl_CHECK_NEXT_HEADERS([fenv.h])
  if test $ac_cv_header_fenv_h = yes; then
    HAVE_FENV_H=1
  else
    HAVE_FENV_H=0
  fi
  AC_SUBST([HAVE_FENV_H])

  dnl Check for declarations of anything we want to poison if the
  dnl corresponding gnulib module is not in use, and which is not
  dnl guaranteed by C99.
  gl_WARN_ON_USE_PREPARE([[#include <fenv.h>
    ]], [fedisableexcept feenableexcept fegetexcept fesetexcept
    fetestexceptflag])
])

# gl_FENV_MODULE_INDICATOR([modulename])
# sets the shell variable that indicates the presence of the given module
# to a C preprocessor expression that will evaluate to 1.
# This macro invocation must not occur in macros that are AC_REQUIREd.
AC_DEFUN([gl_FENV_MODULE_INDICATOR],
[
  dnl Ensure to expand the default settings once only.
  gl_FENV_H_REQUIRE_DEFAULTS
  gl_MODULE_INDICATOR_SET_VARIABLE([$1])
  dnl Define it also as a C macro, for the benefit of the unit tests.
  gl_MODULE_INDICATOR_FOR_TESTS([$1])
])

# Initializes the default values for AC_SUBSTed shell variables.
# This macro must not be AC_REQUIREd.  It must only be invoked, and only
# outside of macros or in macros that are not AC_REQUIREd.
AC_DEFUN([gl_FENV_H_REQUIRE_DEFAULTS],
[
  m4_defun(GL_MODULE_INDICATOR_PREFIX[_FENV_H_MODULE_INDICATOR_DEFAULTS], [
    gl_MODULE_INDICATOR_INIT_VARIABLE([GNULIB_FECLEAREXCEPT])
    gl_MODULE_INDICATOR_INIT_VARIABLE([GNULIB_FEDISABLEEXCEPT])
    gl_MODULE_INDICATOR_INIT_VARIABLE([GNULIB_FEENABLEEXCEPT])
    gl_MODULE_INDICATOR_INIT_VARIABLE([GNULIB_FEGETENV])
    gl_MODULE_INDICATOR_INIT_VARIABLE([GNULIB_FEGETEXCEPT])
    gl_MODULE_INDICATOR_INIT_VARIABLE([GNULIB_FEGETEXCEPTFLAG])
    gl_MODULE_INDICATOR_INIT_VARIABLE([GNULIB_FEGETROUND])
    gl_MODULE_INDICATOR_INIT_VARIABLE([GNULIB_FEHOLDEXCEPT])
    gl_MODULE_INDICATOR_INIT_VARIABLE([GNULIB_FERAISEEXCEPT])
    gl_MODULE_INDICATOR_INIT_VARIABLE([GNULIB_FESETENV])
    gl_MODULE_INDICATOR_INIT_VARIABLE([GNULIB_FESETEXCEPT])
    gl_MODULE_INDICATOR_INIT_VARIABLE([GNULIB_FESETEXCEPTFLAG])
    gl_MODULE_INDICATOR_INIT_VARIABLE([GNULIB_FESETROUND])
    gl_MODULE_INDICATOR_INIT_VARIABLE([GNULIB_FETESTEXCEPT])
    gl_MODULE_INDICATOR_INIT_VARIABLE([GNULIB_FETESTEXCEPTFLAG])
    gl_MODULE_INDICATOR_INIT_VARIABLE([GNULIB_FEUPDATEENV])
  ])
  m4_require(GL_MODULE_INDICATOR_PREFIX[_FENV_H_MODULE_INDICATOR_DEFAULTS])
  AC_REQUIRE([gl_FENV_H_DEFAULTS])
])

AC_DEFUN([gl_FENV_H_DEFAULTS],
[
  dnl Assume proper GNU behavior unless another module says otherwise.
  HAVE_FECLEAREXCEPT=1;              AC_SUBST([HAVE_FECLEAREXCEPT])
  HAVE_FEDISABLEEXCEPT=1;            AC_SUBST([HAVE_FEDISABLEEXCEPT])
  HAVE_FEENABLEEXCEPT=1;             AC_SUBST([HAVE_FEENABLEEXCEPT])
  HAVE_FEGETENV=1;                   AC_SUBST([HAVE_FEGETENV])
  HAVE_FEGETEXCEPT=1;                AC_SUBST([HAVE_FEGETEXCEPT])
  HAVE_FEGETEXCEPTFLAG=1;            AC_SUBST([HAVE_FEGETEXCEPTFLAG])
  HAVE_FEGETROUND=1;                 AC_SUBST([HAVE_FEGETROUND])
  HAVE_FEHOLDEXCEPT=1;               AC_SUBST([HAVE_FEHOLDEXCEPT])
  HAVE_FERAISEEXCEPT=1;              AC_SUBST([HAVE_FERAISEEXCEPT])
  HAVE_FESETENV=1;                   AC_SUBST([HAVE_FESETENV])
  HAVE_FESETEXCEPT=1;                AC_SUBST([HAVE_FESETEXCEPT])
  HAVE_FESETEXCEPTFLAG=1;            AC_SUBST([HAVE_FESETEXCEPTFLAG])
  HAVE_FESETROUND=1;                 AC_SUBST([HAVE_FESETROUND])
  HAVE_FETESTEXCEPT=1;               AC_SUBST([HAVE_FETESTEXCEPT])
  HAVE_FETESTEXCEPTFLAG=1;           AC_SUBST([HAVE_FETESTEXCEPTFLAG])
  HAVE_FEUPDATEENV=1;                AC_SUBST([HAVE_FEUPDATEENV])
  REPLACE_FECLEAREXCEPT=0;           AC_SUBST([REPLACE_FECLEAREXCEPT])
  REPLACE_FEDISABLEEXCEPT=0;         AC_SUBST([REPLACE_FEDISABLEEXCEPT])
  REPLACE_FEENABLEEXCEPT=0;          AC_SUBST([REPLACE_FEENABLEEXCEPT])
  REPLACE_FEGETENV=0;                AC_SUBST([REPLACE_FEGETENV])
  REPLACE_FEGETEXCEPT=0;             AC_SUBST([REPLACE_FEGETEXCEPT])
  REPLACE_FEGETEXCEPTFLAG=0;         AC_SUBST([REPLACE_FEGETEXCEPTFLAG])
  REPLACE_FEGETROUND=0;              AC_SUBST([REPLACE_FEGETROUND])
  REPLACE_FEHOLDEXCEPT=0;            AC_SUBST([REPLACE_FEHOLDEXCEPT])
  REPLACE_FERAISEEXCEPT=0;           AC_SUBST([REPLACE_FERAISEEXCEPT])
  REPLACE_FESETENV=0;                AC_SUBST([REPLACE_FESETENV])
  REPLACE_FESETEXCEPT=0;             AC_SUBST([REPLACE_FESETEXCEPT])
  REPLACE_FESETEXCEPTFLAG=0;         AC_SUBST([REPLACE_FESETEXCEPTFLAG])
  REPLACE_FESETROUND=0;              AC_SUBST([REPLACE_FESETROUND])
  REPLACE_FETESTEXCEPT=0;            AC_SUBST([REPLACE_FETESTEXCEPT])
  REPLACE_FEUPDATEENV=0;             AC_SUBST([REPLACE_FEUPDATEENV])
])
