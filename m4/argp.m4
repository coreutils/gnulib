# argp.m4 serial 5
dnl Copyright (C) 2003-2005 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_ARGP],
[
  AC_REQUIRE([AC_C_INLINE])
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])
  AC_REQUIRE([gl_GETOPT_SUBSTITUTE])
  AC_CHECK_DECLS([program_invocation_name, program_invocation_short_name],,,
    [#include <errno.h>])
  AC_CHECK_DECLS_ONCE(
     [clearerr_unlocked feof_unlocked ferror_unlocked
      fflush_unlocked fgets_unlocked fputc_unlocked fputs_unlocked
      fread_unlocked fwrite_unlocked getc_unlocked
      getchar_unlocked putc_unlocked putchar_unlocked])
  AC_CHECK_FUNCS_ONCE([flockfile funlockfile])
  AC_CHECK_HEADERS_ONCE([features.h linewrap.h])
])
