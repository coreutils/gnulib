# roundf.m4 serial 2
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
  else
    AC_CHECK_DECLS([ceilf, floorf], , , [#include <math.h>])
    if test "$ac_cv_have_decl_floorf" = yes &&
       test "$ac_cv_have_decl_ceilf" = yes; then
      gl_CHECK_MATH_LIB([ROUNDF_LIBM], [x = floorf (x) + ceilf (x);])
    else
      ROUNDF_LIBM=
    fi
    HAVE_DECL_ROUNDF=0
    AC_LIBOBJ([roundf])
  fi
  AC_SUBST([HAVE_DECL_ROUNDF])
  AC_SUBST([ROUNDF_LIBM])
])
