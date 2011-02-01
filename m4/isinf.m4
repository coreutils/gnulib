# isinf.m4 serial 3
dnl Copyright (C) 2007-2011 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_ISINF],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  dnl Persuade glibc <math.h> to declare isinf.
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])
  AC_CHECK_DECLS([isinf], , , [#include <math.h>])
  if test "$ac_cv_have_decl_isinf" = yes; then
    gl_CHECK_MATH_LIB([ISINF_LIBM], [x = isinf (x);])
    if test "$ISINF_LIBM" != missing; then
      dnl Test whether isinf() on 'long double' works.
      gl_ISINFL_WORKS
      case "$gl_cv_func_isinfl_works" in
        *yes) ;;
        *)    ISINF_LIBM=missing;;
      esac
    fi
  fi
  if test "$ac_cv_have_decl_isinf" != yes ||
     test "$ISINF_LIBM" = missing; then
    REPLACE_ISINF=1
    AC_LIBOBJ([isinf])
    ISINF_LIBM=
  fi
  AC_SUBST([ISINF_LIBM])
])

dnl Test whether isinf() correctly returns false for LDBL_MAX.
AC_DEFUN([gl_ISINFL_WORKS],
[
  AC_REQUIRE([AC_PROG_CC])
  AC_REQUIRE([gl_BIGENDIAN])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  AC_CACHE_CHECK([whether isinf(long double) works], [gl_cv_func_isinfl_works],
    [
      AC_RUN_IFELSE([AC_LANG_PROGRAM([[#include <float.h>
                                       #include <math.h>]],
                                     [[return !!isinf(LDBL_MAX);]])],
                    [gl_cv_func_isinfl_works=yes],
                    [gl_cv_func_isinfl_works=no],
                    [gl_cv_func_isinfl_works="guessing no"])
    ])
])
