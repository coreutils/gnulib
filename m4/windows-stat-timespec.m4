# windows-stat-timespec.m4
# serial 2
dnl Copyright (C) 2017-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

dnl Enable precise timestamps in 'struct stat' on native Windows platforms.

AC_DEFUN([gl_WINDOWS_STAT_TIMESPEC],
[
  AC_REQUIRE([AC_CANONICAL_HOST])
  case "$host_os" in
    mingw* | windows*) WINDOWS_STAT_TIMESPEC=1 ;;
    *)                 WINDOWS_STAT_TIMESPEC=0 ;;
  esac
])
