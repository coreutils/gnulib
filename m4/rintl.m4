# rintl.m4 serial 2
dnl Copyright (C) 2011 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_RINTL],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])

  dnl Determine RINTL_LIBM.
  gl_MATHFUNC([rintl], [long double], [(long double)])
  if test $gl_cv_func_rintl_no_libm = no \
     && test $gl_cv_func_rintl_in_libm = no; then
    HAVE_RINTL=0
    RINTL_LIBM=
  fi
  AC_SUBST([RINTL_LIBM])
])
