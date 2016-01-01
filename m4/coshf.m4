# coshf.m4 serial 2
dnl Copyright (C) 2011-2016 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_COSHF],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  AC_REQUIRE([gl_FUNC_COSH])

  dnl Persuade glibc <math.h> to declare coshf().
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])

  dnl Test whether coshf() exists. Assume that coshf(), if it exists, is
  dnl defined in the same library as cosh().
  save_LIBS="$LIBS"
  LIBS="$LIBS $COSH_LIBM"
  AC_CHECK_FUNCS([coshf])
  LIBS="$save_LIBS"
  if test $ac_cv_func_coshf = yes; then
    COSHF_LIBM="$COSH_LIBM"
  else
    HAVE_COSHF=0
    COSHF_LIBM="$COSH_LIBM"
  fi
  AC_SUBST([COSHF_LIBM])
])
