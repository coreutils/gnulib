# imaxdiv.m4 serial 7
dnl Copyright (C) 2006, 2009-2023 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_IMAXDIV],
[
  AC_REQUIRE([gl_INTTYPES_H_DEFAULTS])

  dnl On OSF/1 5.1 with cc, this function is declared but not defined.
  gl_CHECK_FUNCS_ANDROID([imaxdiv], [[#include <inttypes.h>]])
  if test $ac_cv_func_imaxdiv = no; then
    HAVE_IMAXDIV=0
    case "$gl_cv_onwards_func_imaxdiv" in
      future*) REPLACE_IMAXDIV=1 ;;
    esac
  fi
  AC_CHECK_DECLS_ONCE([imaxdiv])
  if test "$ac_cv_have_decl_imaxdiv" != yes; then
    HAVE_DECL_IMAXDIV=0
  fi

  AC_CACHE_CHECK([whether <inttypes.h> defines imaxdiv_t],
    [gl_cv_type_imaxdiv_t],
    [dnl Assume that if imaxdiv is declared, imaxdiv_t is defined.
     if test $ac_cv_have_decl_imaxdiv = yes; then
       gl_cv_type_imaxdiv_t=yes
     else
       AC_COMPILE_IFELSE(
         [AC_LANG_PROGRAM(
            [[#include <inttypes.h>
              imaxdiv_t x;
            ]])
         ],
         [gl_cv_type_imaxdiv_t=yes],
         [gl_cv_type_imaxdiv_t=no])
     fi
    ])
  if test $gl_cv_type_imaxdiv_t = no; then
    HAVE_IMAXDIV_T=0
  fi
])

# Prerequisites of lib/imaxdiv.c.
AC_DEFUN([gl_PREREQ_IMAXDIV], [:])
