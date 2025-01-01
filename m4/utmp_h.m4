# utmp_h.m4
# serial 2
dnl Copyright (C) 2023-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

# Configure a GNU-like replacement for <utmp.h>.

AC_DEFUN_ONCE([gl_UTMP_H],
[
  dnl Ensure to expand the default settings once only, before all statements
  dnl that occur in other macros.
  AC_REQUIRE([gl_UTMP_H_DEFAULTS])

  gl_CHECK_NEXT_HEADERS([utmp.h])
  if test $ac_cv_header_utmp_h = yes; then
    HAVE_UTMP_H=1
  else
    HAVE_UTMP_H=0
  fi
  AC_SUBST([HAVE_UTMP_H])

  gl_PTY_CHECK_UTIL_H

  AC_CHECK_HEADERS_ONCE([termios.h])
  if test $ac_cv_header_termios_h != yes; then
    HAVE_TERMIOS_H=0
  fi

  dnl Check for declarations of anything we want to poison if the
  dnl corresponding gnulib module is not in use, and which is not
  dnl guaranteed by C89.
  gl_WARN_ON_USE_PREPARE([[
      #include <sys/types.h>
      #include <utmp.h>
    ]],
    [login_tty])
])

# gl_UTMP_MODULE_INDICATOR([modulename])
# sets the shell variable that indicates the presence of the given module
# to a C preprocessor expression that will evaluate to 1.
# This macro invocation must not occur in macros that are AC_REQUIREd.
AC_DEFUN([gl_UTMP_MODULE_INDICATOR],
[
  dnl Ensure to expand the default settings once only.
  gl_UTMP_H_REQUIRE_DEFAULTS
  gl_MODULE_INDICATOR_SET_VARIABLE([$1])
  dnl Define it also as a C macro, for the benefit of the unit tests.
  gl_MODULE_INDICATOR_FOR_TESTS([$1])
])

# Initializes the default values for AC_SUBSTed shell variables.
# This macro must not be AC_REQUIREd.  It must only be invoked, and only
# outside of macros or in macros that are not AC_REQUIREd.
AC_DEFUN([gl_UTMP_H_REQUIRE_DEFAULTS],
[
  m4_defun(GL_MODULE_INDICATOR_PREFIX[_UTMP_H_MODULE_INDICATOR_DEFAULTS], [
    gl_MODULE_INDICATOR_INIT_VARIABLE([GNULIB_LOGIN_TTY])
  ])
  m4_require(GL_MODULE_INDICATOR_PREFIX[_UTMP_H_MODULE_INDICATOR_DEFAULTS])
  AC_REQUIRE([gl_UTMP_H_DEFAULTS])
])

AC_DEFUN([gl_UTMP_H_DEFAULTS],
[
  dnl For HAVE_UTIL_H, HAVE_LIBUTIL_H.
  AC_REQUIRE([gl_PTY_H_DEFAULTS])
  dnl For HAVE_TERMIOS_H.
  AC_REQUIRE([gl_TERMIOS_H_DEFAULTS])
  dnl Assume proper GNU behavior unless another module says otherwise.
  HAVE_LOGIN_TTY=1;             AC_SUBST([HAVE_LOGIN_TTY])
  REPLACE_LOGIN_TTY=0;          AC_SUBST([REPLACE_LOGIN_TTY])
])
