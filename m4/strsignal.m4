# strsignal.m4 serial 1
dnl Copyright (C) 2008 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_STRSIGNAL],
[
  dnl Persuade glibc <string.h> to declare strsignal().
  AC_REQUIRE([AC_USE_SYSTEM_EXTENSIONS])

  AC_REQUIRE([AC_DECL_SYS_SIGLIST])

  AC_REQUIRE([gl_HEADER_STRING_H_DEFAULTS])
  AC_REPLACE_FUNCS([strsignal])
  AC_CHECK_DECLS_ONCE([strsignal])
  if test $ac_cv_have_decl_strsignal = no; then
    HAVE_DECL_STRSIGNAL=0
    gl_PREREQ_STRSIGNAL
  fi
])

# Prerequisites of lib/strsignal.c.
AC_DEFUN([gl_PREREQ_STRSIGNAL], [:])
