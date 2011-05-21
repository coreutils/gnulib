# strerror.m4 serial 11
dnl Copyright (C) 2002, 2007-2011 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_STRERROR],
[
  AC_REQUIRE([gl_HEADER_STRING_H_DEFAULTS])
  AC_REQUIRE([gl_HEADER_ERRNO_H])
  if test -z "$ERRNO_H"; then
    AC_CACHE_CHECK([for working strerror function],
     [gl_cv_func_working_strerror],
     [AC_RUN_IFELSE(
        [AC_LANG_PROGRAM(
           [[#include <string.h>
             #include <errno.h>
           ]],
           [[int result = 0;
             if (!*strerror (-2)) result |= 1;
             errno = 0;
             if (!*strerror (0)) result |= 2;
             if (errno) result |= 4;
             return result;]])],
        [gl_cv_func_working_strerror=yes],
        [gl_cv_func_working_strerror=no],
        [dnl Be pessimistic on cross-compiles for now.
         gl_cv_func_working_strerror=no])
    ])
    if test $gl_cv_func_working_strerror = no; then
      dnl The system's strerror() fails to return a string for out-of-range
      dnl integers. Replace it.
      REPLACE_STRERROR=1
    fi
  else
    dnl The system's strerror() cannot know about the new errno values we add
    dnl to <errno.h>. Replace it.
    REPLACE_STRERROR=1
  fi
  if test $REPLACE_STRERROR = 1; then
    AC_LIBOBJ([strerror])
    gl_PREREQ_STRERROR
  fi
])

# Prerequisites of lib/strerror.c.
AC_DEFUN([gl_PREREQ_STRERROR], [
  AC_CHECK_DECLS([strerror])
  AC_CHECK_HEADERS_ONCE([sys/socket.h])
  if test $ac_cv_header_sys_socket_h != yes; then
    dnl We cannot use AC_CHECK_HEADERS_ONCE here, because that would make
    dnl the check for those headers unconditional; yet cygwin reports
    dnl that the headers are present but cannot be compiled (since on
    dnl cygwin, all socket information should come from sys/socket.h).
    AC_CHECK_HEADERS([winsock2.h])
  fi
])
