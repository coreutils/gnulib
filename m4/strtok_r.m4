# strtok_r.m4 serial 8
dnl Copyright (C) 2002, 2003, 2004, 2007, 2009 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_STRTOK_R],
[
  dnl The strtok_r() declaration in lib/string.in.h uses 'restrict'.
  AC_REQUIRE([AC_C_RESTRICT])

  AC_REQUIRE([gl_HEADER_STRING_H_DEFAULTS])
  AC_REPLACE_FUNCS([strtok_r])
  AC_CHECK_DECLS_ONCE([strtok_r])
  if test $ac_cv_have_decl_strtok_r = no; then
    HAVE_DECL_STRTOK_R=0
  fi
  gl_PREREQ_STRTOK_R
])

# Prerequisites of lib/strtok_r.c.
AC_DEFUN([gl_PREREQ_STRTOK_R], [
  :
])
