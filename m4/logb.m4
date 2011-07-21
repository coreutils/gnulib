# logb.m4 serial 3
dnl Copyright (C) 2010-2011 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_LOGB],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  dnl Test whether logb() is declared.
  AC_CHECK_DECLS([logb], , , [#include <math.h>])
  if test "$ac_cv_have_decl_logb" != yes; then
    HAVE_DECL_LOGB=0
  fi
  dnl Test whether logb() can be used without libm.
  LOGB_LIBM=?
  AC_LINK_IFELSE(
    [AC_LANG_PROGRAM(
       [[#ifndef __NO_MATH_INLINES
         # define __NO_MATH_INLINES 1 /* for glibc */
         #endif
         #include <math.h>
         extern
         #ifdef __cplusplus
         "C"
         #endif
         double logb (double x);
         double x;]],
       [[x = logb(x);]])],
    [LOGB_LIBM=])
  if test "$LOGB_LIBM" = "?"; then
    save_LIBS="$LIBS"
    LIBS="$LIBS -lm"
    AC_LINK_IFELSE(
      [AC_LANG_PROGRAM(
         [[#ifndef __NO_MATH_INLINES
           # define __NO_MATH_INLINES 1 /* for glibc */
           #endif
           #include <math.h>
           extern
           #ifdef __cplusplus
           "C"
           #endif
           double logb (double x);
           double x;]],
         [[x = logb(x);]])],
      [LOGB_LIBM="-lm"])
    LIBS="$save_LIBS"
  fi
  if test "$LOGB_LIBM" = "?"; then
    LOGB_LIBM=
  fi
  AC_SUBST([LOGB_LIBM])
])
