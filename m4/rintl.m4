# rintl.m4 serial 4
dnl Copyright (C) 2011-2016 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_RINTL],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  AC_REQUIRE([gl_LONG_DOUBLE_VS_DOUBLE])

  dnl Persuade glibc <math.h> to declare rintl().
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])

  dnl Determine RINTL_LIBM.
  gl_MATHFUNC([rintl], [long double], [(long double)])
  if test $gl_cv_func_rintl_no_libm = no \
     && test $gl_cv_func_rintl_in_libm = no; then
    HAVE_RINTL=0
    dnl Find libraries needed to link lib/rintl.c.
    if test $HAVE_SAME_LONG_DOUBLE_AS_DOUBLE = 1; then
      AC_REQUIRE([gl_FUNC_RINT])
      RINTL_LIBM="$RINT_LIBM"
    else
      RINTL_LIBM=
    fi
  fi
  AC_SUBST([RINTL_LIBM])
])
