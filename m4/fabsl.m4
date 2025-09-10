# fabsl.m4
# serial 6
dnl Copyright (C) 2011-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN([gl_FUNC_FABSL],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  AC_REQUIRE([gl_LONG_DOUBLE_VS_DOUBLE])

  dnl Persuade glibc <math.h> to declare fabsl().
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])

  dnl Determine FABSL_LIBM.
  gl_MATHFUNC([fabsl], [long double], [(long double)])
  if test $gl_cv_func_fabsl_no_libm != yes \
     && test $gl_cv_func_fabsl_in_libm != yes; then
    HAVE_FABSL=0
  fi
  if test $HAVE_FABSL = 0; then
    dnl Find libraries needed to link lib/fabsl.c.
    if test $HAVE_SAME_LONG_DOUBLE_AS_DOUBLE = 1; then
      AC_REQUIRE([gl_FUNC_FABS])
      FABSL_LIBM="$FABS_LIBM"
    else
      FABSL_LIBM=
    fi
  fi
  AC_SUBST([FABSL_LIBM])
])
