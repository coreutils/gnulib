# abort-debug.m4
# serial 3
dnl Copyright (C) 2024-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN([gl_ABORT_DEBUG],
[
  AC_REQUIRE([gl_STDLIB_H_DEFAULTS])
  AC_REQUIRE([gl_STACK_TRACE_EARLY])
  if test $CAN_PRINT_STACK_TRACE = 1; then
    REPLACE_ABORT=1
    AC_REQUIRE([AC_C_INLINE])
  fi
])
