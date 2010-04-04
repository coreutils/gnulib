# trunc.m4 serial 3
dnl Copyright (C) 2007, 2010 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_TRUNC],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  dnl Persuade glibc <math.h> to declare trunc().
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])
  dnl Test whether trunc() is declared.
  AC_CHECK_DECLS([trunc], , , [#include <math.h>])
  if test "$ac_cv_have_decl_trunc" = yes; then
    dnl Test whether trunc() can be used without libm.
    TRUNC_LIBM=?
    AC_TRY_LINK([
       #ifndef __NO_MATH_INLINES
       # define __NO_MATH_INLINES 1 /* for glibc */
       #endif
       #include <math.h>
       double x;],
      [x = trunc(x);],
      [TRUNC_LIBM=])
    if test "$TRUNC_LIBM" = "?"; then
      save_LIBS="$LIBS"
      LIBS="$LIBS -lm"
      AC_TRY_LINK([
         #ifndef __NO_MATH_INLINES
         # define __NO_MATH_INLINES 1 /* for glibc */
         #endif
         #include <math.h>
         double x;],
        [x = trunc(x);],
        [TRUNC_LIBM="-lm"])
      LIBS="$save_LIBS"
    fi
    if test "$TRUNC_LIBM" = "?"; then
      TRUNC_LIBM=
    fi
  else
    HAVE_DECL_TRUNC=0
    AC_LIBOBJ([trunc])
    TRUNC_LIBM=
  fi
  AC_SUBST([TRUNC_LIBM])
])
