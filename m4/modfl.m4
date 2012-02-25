# modfl.m4 serial 1
dnl Copyright (C) 2011-2012 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_MODFL],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  AC_REQUIRE([gl_LONG_DOUBLE_VS_DOUBLE])

  dnl Test whether modfl() exists. We cannot assume that modfl(), if it
  dnl exists, is defined in the same library as modf(). This is not the case
  dnl on FreeBSD, Solaris.
  gl_MATHFUNC([modfl], [long double], [(long double, long double *)])
  if test $gl_cv_func_modfl_no_libm = no \
     && test $gl_cv_func_modfl_in_libm = no; then
    HAVE_MODFL=0
    if test $HAVE_SAME_LONG_DOUBLE_AS_DOUBLE = 1; then
      AC_REQUIRE([gl_FUNC_MODF])
      MODFL_LIBM="$MODF_LIBM"
    else
      AC_REQUIRE([gl_FUNC_TRUNCL])
      MODFL_LIBM="$TRUNCL_LIBM"
    fi
  fi
  AC_SUBST([MODFL_LIBM])
])
