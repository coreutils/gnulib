# rintf.m4 serial 2
dnl Copyright (C) 2011 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_RINTF],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])

  dnl Determine RINTF_LIBM.
  gl_MATHFUNC([rintf], [float], [(float)])
  if test $gl_cv_func_rintf_no_libm = no \
     && test $gl_cv_func_rintf_in_libm = no; then
    HAVE_RINTF=0
    RINTF_LIBM=
  fi
  AC_SUBST([RINTF_LIBM])
])
