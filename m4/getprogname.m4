# getprogname.m4 - check for getprogname or replacements for it

# Copyright (C) 2016 Free Software Foundation, Inc.
# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# serial 3

AC_DEFUN([gl_FUNC_GETPROGNAME],
[
  AC_CHECK_FUNCS_ONCE([getprogname getexecname])
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])
  AC_CHECK_DECLS([program_invocation_name], [], [], [#include <errno.h>])
  AC_CHECK_DECLS([program_invocation_short_name], [], [], [#include <errno.h>])
  AC_CHECK_DECLS([__argv], [], [], [#include <stdlib.h>])
  AC_CHECK_DECLS([__progname], [], [], [char *__progname;])
])
