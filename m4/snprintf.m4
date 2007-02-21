# snprintf.m4 serial 3
dnl Copyright (C) 2002, 2003, 2004, 2007 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_SNPRINTF],
[
  AC_REQUIRE([gl_STDIO_H_DEFAULTS])
  AC_REPLACE_FUNCS(snprintf)
  AC_CHECK_DECLS_ONCE(snprintf)
  if test $ac_cv_have_decl_snprintf = no; then
    HAVE_DECL_SNPRINTF=0
  fi
  gl_PREREQ_SNPRINTF
])

# Prerequisites of lib/snprintf.c.
AC_DEFUN([gl_PREREQ_SNPRINTF], [:])
