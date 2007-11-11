# roundf.m4 serial 4
dnl Copyright (C) 2007 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_ROUNDF],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  dnl Persuade glibc <math.h> to declare roundf().
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])
  AC_CHECK_DECLS([roundf], , , [#include <math.h>])
  if test "$ac_cv_have_decl_roundf" = yes; then
    gl_CHECK_MATH_LIB([ROUNDF_LIBM], [x = roundf (x);])
  fi
  if test "$ac_cv_have_decl_roundf" != yes || test "$ROUNDF_LIBM" = missing; then
    REPLACE_ROUNDF=1
    AC_LIBOBJ([roundf])
    AC_CHECK_DECLS([ceilf, floorf], , , [#include <math.h>])
    if test "$ac_cv_have_decl_floorf" = yes \
       && test "$ac_cv_have_decl_ceilf" = yes; then
      gl_CHECK_MATH_LIB([ROUNDF_LIBM], [x = floorf (x) + ceilf (x);])
      if test "$ROUNDF_LIBM" != missing; then
        AC_DEFINE([HAVE_FLOORF_AND_CEILF], 1,
          [Define if the both the floorf() and ceilf() functions exist.])
      else
        ROUNDF_LIBM=
      fi
    else
      ROUNDF_LIBM=
    fi
  fi
  AC_SUBST([ROUNDF_LIBM])
])
