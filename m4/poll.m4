# poll.m4 serial 3
dnl Copyright (c) 2003 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_POLL],
[
  AC_CHECK_HEADERS(poll.h)
  if test "$ac_cv_header_poll_h" = no; then
    POLL_H=poll.h
  else
    POLL_H=
  fi
  AC_SUBST([POLL_H])

  AC_REPLACE_FUNCS(poll)
  if test $ac_cv_func_poll = no; then
    gl_PREREQ_POLL
  fi
])

# Prerequisites of lib/poll.c.
AC_DEFUN([gl_PREREQ_POLL],
[
  AC_REQUIRE([AC_HEADER_TIME])
  AC_CHECK_HEADERS_ONCE(sys/time.h)
])
