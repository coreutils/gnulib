# Check for stdnoreturn.h.

dnl Copyright 2011 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_STDNORETURN_H],
[
  dnl Assume that a system-supplied stdnoreturn.h works if it exists.
  AC_CHECK_HEADERS_ONCE([stdnoreturn.h])

  if test "$ac_cv_header_stdnoreturn_h" = yes; then
    STDNORETURN_H=''
  else
    STDNORETURN_H='stdnoreturn.h'
  fi
  AC_SUBST([STDNORETURN_H])

  AM_CONDITIONAL([GL_GENERATE_STDNORETURN_H],
    [test $ac_cv_header_stdnoreturn_h != yes])
])
