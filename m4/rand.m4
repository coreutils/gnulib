# rand.m4
# serial 1
dnl Copyright (C) 2023-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN([gl_FUNC_RAND],
[
  AC_REQUIRE([gl_STDLIB_H_DEFAULTS])
  AC_REQUIRE([AC_CANONICAL_HOST])

  dnl On CheriBSD, rand() lacks locking, leading to an out-of-bounds read
  dnl inside random_r.
  case "$host" in
    aarch64c-*-freebsd*) REPLACE_RAND=1 ;;
  esac
])
