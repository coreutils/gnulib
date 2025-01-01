# stat-size.m4
# serial 1
dnl Copyright (C) 2011-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN([gl_STAT_SIZE],
[
  # Don't call AC_STRUCT_ST_BLOCKS because it causes bugs.  Details at
  # https://lists.gnu.org/r/bug-gnulib/2011-06/msg00051.html
  AC_CHECK_HEADERS_ONCE([sys/param.h])
])
