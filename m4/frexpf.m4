# frexpf.m4 serial 2
dnl Copyright (C) 2011 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_FREXPF],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  AC_REQUIRE([gl_FUNC_FREXP])

  dnl Test whether frexpf() exists. We cannot assume that frexpf(), if it
  dnl exists, is defined in the same library as frexp(). This is not the case
  dnl on NetBSD, OpenBSD.
  gl_MATHFUNC([frexpf], [float], [(float, int *)])
  if test $gl_cv_func_frexpf_no_libm = no \
     && test $gl_cv_func_frexpf_in_libm = no; then
    HAVE_FREXPF=0
    FREXPF_LIBM="$FREXP_LIBM"
  fi
  AC_SUBST([FREXPF_LIBM])
])
