# memmem.m4 serial 2
dnl Copyright (C) 2002, 2003, 2004 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_MEMMEM],
[
  dnl Persuade glibc <string.h> to declare memmem().
  AC_REQUIRE([AC_GNU_SOURCE])

  AC_REPLACE_FUNCS(memmem)
  AC_CHECK_DECLS_ONCE(memmem)
  gl_PREREQ_MEMMEM
])

# Prerequisites of lib/memmem.c.
AC_DEFUN([gl_PREREQ_MEMMEM], [:])
