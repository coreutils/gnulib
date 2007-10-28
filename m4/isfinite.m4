# isfinite.m4 serial 1
dnl Copyright (C) 2007 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_ISFINITE],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  dnl Persuade glibc <math.h> to declare isfinite.
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])
  AC_CHECK_DECLS([isfinite], , , [#include <math.h>])
  if test "$ac_cv_have_decl_isfinite" = yes; then
    gl_CHECK_MATH_LIB([ISFINITE_LIBM], [x = isfinite (x);])
  fi
  if test "$ac_cv_have_decl_isfinite" != yes ||
     test "$ISFINITE_LIBM" = missing; then
    ISFINITE_LIBM=
    HAVE_DECL_ISFINITE=0
    AC_LIBOBJ([isfinite])
  fi
  AC_SUBST([HAVE_DECL_ISFINITE])
  AC_SUBST([ISFINITE_LIBM])
])
