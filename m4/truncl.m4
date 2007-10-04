# truncl.m4 serial 1
dnl Copyright (C) 2007 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_TRUNCL],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  dnl Persuade glibc <math.h> to declare truncl().
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])
  dnl Test whether truncl() is declared.
  AC_CHECK_DECLS([truncl], , , [#include <math.h>])
  if test "$ac_cv_have_decl_truncl" = yes; then
    dnl Test whether truncl() can be used without libm.
    TRUNCL_LIBM=?
    AC_TRY_LINK([
       #ifndef __NO_MATH_INLINES
       # define __NO_MATH_INLINES 1 /* for glibc */
       #endif
       #include <math.h>
       long double x;],
      [x = truncl(x);],
      [TRUNCL_LIBM=])
    if test "$TRUNCL_LIBM" = "?"; then
      save_LIBS="$LIBS"
      LIBS="$LIBS -lm"
      AC_TRY_LINK([
         #ifndef __NO_MATH_INLINES
         # define __NO_MATH_INLINES 1 /* for glibc */
         #endif
         #include <math.h>
         long double x;],
        [x = truncl(x);],
        [TRUNCL_LIBM="-lm"])
      LIBS="$save_LIBS"
    fi
    if test "$TRUNCL_LIBM" = "?"; then
      TRUNCL_LIBM=
    fi
  else
    HAVE_DECL_TRUNCL=0
    AC_LIBOBJ([truncl])
    TRUNCL_LIBM=
  fi
  AC_SUBST([HAVE_DECL_TRUNCL])
  AC_SUBST([TRUNCL_LIBM])
])
