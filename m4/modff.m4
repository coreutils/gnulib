# modff.m4 serial 2
dnl Copyright (C) 2011 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_MODFF],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  AC_REQUIRE([gl_FUNC_MODF])

  dnl Test whether modff() exists. We cannot assume that modff(), if it
  dnl exists, is defined in the same library as modf(). This is not the case
  dnl on FreeBSD, NetBSD, OpenBSD.
  gl_MATHFUNC([modff], [float], [(float, float *)])
  if test $gl_cv_func_modff_no_libm = no \
     && test $gl_cv_func_modff_in_libm = no; then
    HAVE_MODFF=0
    MODFF_LIBM="$MODF_LIBM"
  fi
  AC_SUBST([MODFF_LIBM])
])
