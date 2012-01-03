# isatty.m4 serial 1
dnl Copyright (C) 2012 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_ISATTY],
[
  AC_REQUIRE([gl_UNISTD_H_DEFAULTS])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  dnl On native Windows, the system's isatty() returns true for pipes and
  dnl for the NUL device.
  case $host_os in
    mingw*) REPLACE_ISATTY=1 ;;
  esac
])
