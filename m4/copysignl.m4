# copysignl.m4 serial 1
dnl Copyright (C) 2011 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_COPYSIGNL],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])

  dnl Determine COPYSIGNL_LIBM.
  gl_MATHFUNC([copysignl], [long double], [(long double, long double)])
  if test $gl_cv_func_copysignl_no_libm = no \
     && test $gl_cv_func_copysignl_in_libm = no; then
    HAVE_COPYSIGNL=0
    COPYSIGNL_LIBM=
  fi
  AC_SUBST([COPYSIGNL_LIBM])
])
