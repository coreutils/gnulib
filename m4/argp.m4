# argp.m4 serial 4
dnl Copyright (C) 2003, 2004 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_ARGP],
[
  AC_REQUIRE([AC_C_INLINE])
  AC_REQUIRE([gl_FUNC_GLIBC_UNLOCKED_IO])
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])
  AC_REQUIRE([gl_GETOPT_SUBSTITUTE])
  AC_CHECK_DECLS([program_invocation_name, program_invocation_short_name],,,
    [#include <errno.h>])
  AC_CHECK_FUNCS_ONCE([flockfile funlockfile])
  AC_CHECK_HEADERS_ONCE([features.h linewrap.h])
])
