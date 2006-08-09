# strtok_r.m4 serial 3
dnl Copyright (C) 2002, 2003, 2004 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_STRTOK_R],
[
  AC_REPLACE_FUNCS(strtok_r)
  AC_CHECK_DECLS_ONCE(strtok_r)
  gl_PREREQ_STRTOK_R
])

# Prerequisites of lib/strtok_r.h and lib/strtok_r.c.
AC_DEFUN([gl_PREREQ_STRTOK_R], [
  AC_REQUIRE([AC_C_RESTRICT])
])
