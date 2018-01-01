# monetary_h.m4 serial 1
dnl Copyright (C) 2017-2018 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_MONETARY_H],
[
  dnl Use AC_REQUIRE here, so that the default behavior below is expanded
  dnl once only, before all statements that occur in other macros.
  AC_REQUIRE([gl_MONETARY_H_BODY])
])

AC_DEFUN([gl_MONETARY_H_BODY],
[
  AC_REQUIRE([gl_MONETARY_H_DEFAULTS])

  AC_CHECK_HEADERS_ONCE([monetary.h])
  dnl For now, we provide a <monetary.h> wrapper only if the system already
  dnl has a <monetary.h>.
  if test $ac_cv_header_monetary_h = yes; then
    MONETARY_H='monetary.h'

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
  else
    MONETARY_H=''
  fi
  AC_SUBST([MONETARY_H])
  AM_CONDITIONAL([GL_GENERATE_MONETARY_H], [test -n "$MONETARY_H"])
])

AC_DEFUN([gl_MONETARY_MODULE_INDICATOR],
[
  dnl Use AC_REQUIRE here, so that the default settings are expanded once only.
  AC_REQUIRE([gl_MONETARY_H_DEFAULTS])
  gl_MODULE_INDICATOR_SET_VARIABLE([$1])
])

AC_DEFUN([gl_MONETARY_H_DEFAULTS],
[
  GNULIB_STRFMON_L=0;      AC_SUBST([GNULIB_STRFMON_L])
  dnl Assume proper GNU behavior unless another module says otherwise.
  HAVE_STRFMON_L=1;        AC_SUBST([HAVE_STRFMON_L])
  REPLACE_STRFMON_L=0;     AC_SUBST([REPLACE_STRFMON_L])
])
