# perror.m4 serial 2
dnl Copyright (C) 2008-2011 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_PERROR],
[
  AC_REQUIRE([gl_STDIO_H_DEFAULTS])
  AC_REQUIRE([gl_HEADER_ERRNO_H])
  AC_REQUIRE([gl_FUNC_STRERROR_R])
  if test -n "$ERRNO_H"; then
    dnl The system's perror() cannot know about the new errno values we add
    dnl to <errno.h>. Replace it.
    REPLACE_PERROR=1
  fi
  case ${gl_cv_func_strerror_r_works-unset} in
    unset|*yes) ;;
    *) dnl The system's perror() probably inherits the bugs in the
       dnl system's strerror_r(). Replace it.
      REPLACE_PERROR=1 ;;
  esac
])
