# execve.m4
# serial 2
dnl Copyright (C) 2020-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN([gl_FUNC_EXECVE],
[
  AC_REQUIRE([gl_UNISTD_H_DEFAULTS])
  AC_REQUIRE([AC_CANONICAL_HOST])

  case "$host_os" in
    mingw* | windows*) REPLACE_EXECVE=1 ;;
  esac
])
