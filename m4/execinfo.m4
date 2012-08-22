# Check for GNU-style execinfo.h.

dnl Copyright 2012 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_EXECINFO_H],
[
  AC_CHECK_HEADERS_ONCE([execinfo.h])

  if test $ac_cv_header_execinfo_h = yes; then
    EXECINFO_H=''
  else
    EXECINFO_H='execinfo.h'
    AC_LIBOBJ([execinfo])
  fi

  AC_SUBST([EXECINFO_H])
  AM_CONDITIONAL([GL_GENERATE_EXECINFO_H], [test -n "$EXECINFO_H"])
])
