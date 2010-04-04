# truncf.m4 serial 2
dnl Copyright (C) 2007, 2010 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_TRUNCF],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  dnl Persuade glibc <math.h> to declare truncf().
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])
  dnl Test whether truncf() is declared.
  AC_CHECK_DECLS([truncf], , , [#include <math.h>])
  if test "$ac_cv_have_decl_truncf" = yes; then
    dnl Test whether truncf() can be used without libm.
    TRUNCF_LIBM=?
    AC_TRY_LINK([
       #ifndef __NO_MATH_INLINES
       # define __NO_MATH_INLINES 1 /* for glibc */
       #endif
       #include <math.h>
       float x;],
      [x = truncf(x);],
      [TRUNCF_LIBM=])
    if test "$TRUNCF_LIBM" = "?"; then
      save_LIBS="$LIBS"
      LIBS="$LIBS -lm"
      AC_TRY_LINK([
         #ifndef __NO_MATH_INLINES
         # define __NO_MATH_INLINES 1 /* for glibc */
         #endif
         #include <math.h>
         float x;],
        [x = truncf(x);],
        [TRUNCF_LIBM="-lm"])
      LIBS="$save_LIBS"
    fi
    if test "$TRUNCF_LIBM" = "?"; then
      TRUNCF_LIBM=
    fi
  else
    HAVE_DECL_TRUNCF=0
    AC_LIBOBJ([truncf])
    TRUNCF_LIBM=
  fi
  AC_SUBST([TRUNCF_LIBM])
])
