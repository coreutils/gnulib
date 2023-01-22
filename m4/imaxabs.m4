# imaxabs.m4 serial 6
dnl Copyright (C) 2006, 2009-2023 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_IMAXABS],
[
  AC_REQUIRE([gl_INTTYPES_H_DEFAULTS])

  dnl On OSF/1 5.1 with cc, this function is declared but not defined.
  gl_CHECK_FUNCS_ANDROID([imaxabs], [[#include <inttypes.h>]])
  if test $ac_cv_func_imaxabs = no; then
    HAVE_IMAXABS=0
    case "$gl_cv_onwards_func_imaxabs" in
      future*) REPLACE_IMAXABS=1 ;;
    esac
  fi
  AC_CHECK_DECLS_ONCE([imaxabs])
  if test "$ac_cv_have_decl_imaxabs" != yes; then
    HAVE_DECL_IMAXABS=0
  fi
])

# Prerequisites of lib/imaxabs.c.
AC_DEFUN([gl_PREREQ_IMAXABS], [:])
