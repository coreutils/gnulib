# strerror.m4 serial 15
dnl Copyright (C) 2002, 2007-2011 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_STRERROR],
[
  AC_REQUIRE([gl_HEADER_STRING_H_DEFAULTS])
  AC_REQUIRE([gl_HEADER_ERRNO_H])
  m4_ifdef([gl_FUNC_STRERROR_R_WORKS], [
    AC_REQUIRE([gl_FUNC_STRERROR_R_WORKS])
  ])
  if test -z "$ERRNO_H"; then
    AC_CACHE_CHECK([for working strerror function],
     [gl_cv_func_working_strerror],
     [AC_RUN_IFELSE(
        [AC_LANG_PROGRAM(
           [[#include <string.h>
             #include <errno.h>
           ]],
           [[int result = 0;
             char *str;
             if (!*strerror (-2)) result |= 1;
             errno = 0;
             str = strerror (0);
             if (!*str) result |= 2;
             if (errno) result |= 4;
             if (strstr (str, "nknown") || strstr (str, "ndefined"))
               result |= 8;
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
    m4_ifdef([gl_FUNC_STRERROR_R_WORKS], [
      dnl If the system's strerror_r or __xpg_strerror_r clobbers strerror's
      dnl buffer, we must replace strerror.
      case "$gl_cv_func_strerror_r_works" in
        *no) REPLACE_STRERROR=1 ;;
      esac
    ])
  else
    dnl The system's strerror() cannot know about the new errno values we add
    dnl to <errno.h>. Replace it.
    REPLACE_STRERROR=1
  fi
])
