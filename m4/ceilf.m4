# ceilf.m4 serial 1
dnl Copyright (C) 2007 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_CEILF],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  dnl Persuade glibc <math.h> to declare ceilf().
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])
  dnl Test whether ceilf() is declared.
  AC_CHECK_DECLS([ceilf], , , [#include <math.h>])
  if test "$ac_cv_have_decl_ceilf" = yes; then
    dnl Test whether ceilf() can be used without libm.
    CEILF_LIBM=?
    AC_TRY_LINK([
       #ifndef __NO_MATH_INLINES
       # define __NO_MATH_INLINES 1 /* for glibc */
       #endif
       #include <math.h>
       float x;],
      [x = ceilf(x);],
      [CEILF_LIBM=])
    if test "$CEILF_LIBM" = "?"; then
      save_LIBS="$LIBS"
      LIBS="$LIBS -lm"
      AC_TRY_LINK([
         #ifndef __NO_MATH_INLINES
         # define __NO_MATH_INLINES 1 /* for glibc */
         #endif
         #include <math.h>
         float x;],
        [x = ceilf(x);],
        [CEILF_LIBM="-lm"])
      LIBS="$save_LIBS"
    fi
    if test "$CEILF_LIBM" = "?"; then
      CEILF_LIBM=
    fi
  else
    HAVE_DECL_CEILF=0
    AC_LIBOBJ([ceilf])
    CEILF_LIBM=
  fi
  AC_SUBST([HAVE_DECL_CEILF])
  AC_SUBST([CEILF_LIBM])
])
