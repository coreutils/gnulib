# monetary_h.m4 serial 9
dnl Copyright (C) 2017-2022 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN_ONCE([gl_MONETARY_H],
[
  AC_REQUIRE([gl_MONETARY_H_DEFAULTS])
  m4_ifdef([gl_ANSI_CXX], [AC_REQUIRE([gl_ANSI_CXX])])

  AC_CHECK_HEADERS_ONCE([monetary.h])
  dnl For now, we provide a <monetary.h> wrapper only if
  dnl - module 'posixcheck' is present, or
  dnl - C++ GNULIB_NAMESPACE support may be requested, or
  dnl - the system already has a <monetary.h>.
  if m4_ifdef([gl_POSIXCHECK], [true], [m4_ifdef([gl_ANSI_CXX], [test "$CXX" != no], [false]) || test $ac_cv_header_monetary_h = yes]); then
    GL_GENERATE_MONETARY_H=true

    gl_CHECK_NEXT_HEADERS([monetary.h])
    if test $ac_cv_header_monetary_h = yes; then
      HAVE_MONETARY_H=1
    else
      HAVE_MONETARY_H=0
    fi
    AC_SUBST([HAVE_MONETARY_H])

    AC_CHECK_HEADERS_ONCE([xlocale.h])
    if test $ac_cv_header_xlocale_h = yes; then
      HAVE_XLOCALE_H=1
    else
      HAVE_XLOCALE_H=0
    fi
    AC_SUBST([HAVE_XLOCALE_H])

    dnl Check for declarations of anything we want to poison if the
    dnl corresponding gnulib module is not in use.
    gl_WARN_ON_USE_PREPARE([[
      #include <monetary.h>
      ]], [strfmon_l])

    AC_REQUIRE([AC_C_RESTRICT])
  else
    GL_GENERATE_MONETARY_H=false
  fi
])

# gl_MONETARY_MODULE_INDICATOR([modulename])
# sets the shell variable that indicates the presence of the given module
# to a C preprocessor expression that will evaluate to 1.
# This macro invocation must not occur in macros that are AC_REQUIREd.
AC_DEFUN([gl_MONETARY_MODULE_INDICATOR],
[
  dnl Ensure to expand the default settings once only.
  gl_MONETARY_H_REQUIRE_DEFAULTS
  gl_MODULE_INDICATOR_SET_VARIABLE([$1])
])

# Initializes the default values for AC_SUBSTed shell variables.
# This macro must not be AC_REQUIREd.  It must only be invoked, and only
# outside of macros or in macros that are not AC_REQUIREd.
AC_DEFUN([gl_MONETARY_H_REQUIRE_DEFAULTS],
[
  m4_defun(GL_MODULE_INDICATOR_PREFIX[_MONETARY_H_MODULE_INDICATOR_DEFAULTS], [
    gl_MODULE_INDICATOR_INIT_VARIABLE([GNULIB_STRFMON_L])
  ])
  m4_require(GL_MODULE_INDICATOR_PREFIX[_MONETARY_H_MODULE_INDICATOR_DEFAULTS])
  AC_REQUIRE([gl_MONETARY_H_DEFAULTS])
])

AC_DEFUN([gl_MONETARY_H_DEFAULTS],
[
  dnl Assume proper GNU behavior unless another module says otherwise.
  HAVE_STRFMON_L=1;        AC_SUBST([HAVE_STRFMON_L])
  REPLACE_STRFMON_L=0;     AC_SUBST([REPLACE_STRFMON_L])
])
