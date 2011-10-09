# copysignf.m4 serial 1
dnl Copyright (C) 2011 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_COPYSIGNF],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])

  dnl Determine COPYSIGNF_LIBM.
  gl_MATHFUNC([copysignf], [float], [(float, float)])
  if test $gl_cv_func_copysignf_no_libm = no \
     && test $gl_cv_func_copysignf_in_libm = no; then
    HAVE_COPYSIGNF=0
    COPYSIGNF_LIBM=
  fi
  AC_SUBST([COPYSIGNF_LIBM])
])
