# roundl.m4 serial 4
dnl Copyright (C) 2007 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_ROUNDL],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  dnl Persuade glibc <math.h> to declare roundl().
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])
  AC_CHECK_DECLS([roundl], , , [#include <math.h>])
  if test "$ac_cv_have_decl_roundl" = yes; then
    gl_CHECK_MATH_LIB([ROUNDL_LIBM], [x = roundl (x);])
  fi
  if test "$ac_cv_have_decl_roundl" != yes || test "$ROUNDL_LIBM" = missing; then
    REPLACE_ROUNDL=1
    AC_LIBOBJ([roundl])
    AC_CHECK_DECLS([ceill, floorl], , , [#include <math.h>])
    if test "$ac_cv_have_decl_floorl" = yes \
       && test "$ac_cv_have_decl_ceill" = yes; then
      gl_CHECK_MATH_LIB([ROUNDL_LIBM], [x = floorl (x) + ceill (x);])
      if test "$ROUNDL_LIBM" != missing; then
        AC_DEFINE([HAVE_FLOORL_AND_CEILL], 1,
          [Define if the both the floorl() and ceill() functions exist.])
      else
        ROUNDL_LIBM=
      fi
    else
      ROUNDL_LIBM=
    fi
  fi
  AC_SUBST([ROUNDL_LIBM])
])
