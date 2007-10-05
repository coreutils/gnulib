# floorf.m4 serial 1
dnl Copyright (C) 2007 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_FLOORF],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  dnl Persuade glibc <math.h> to declare floorf().
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])
  dnl Test whether floorf() is declared.
  AC_CHECK_DECLS([floorf], , , [#include <math.h>])
  if test "$ac_cv_have_decl_floorf" = yes; then
    dnl Test whether floorf() can be used without libm.
    FLOORF_LIBM=?
    AC_TRY_LINK([
       #ifndef __NO_MATH_INLINES
       # define __NO_MATH_INLINES 1 /* for glibc */
       #endif
       #include <math.h>
       float x;],
      [x = floorf(x);],
      [FLOORF_LIBM=])
    if test "$FLOORF_LIBM" = "?"; then
      save_LIBS="$LIBS"
      LIBS="$LIBS -lm"
      AC_TRY_LINK([
         #ifndef __NO_MATH_INLINES
         # define __NO_MATH_INLINES 1 /* for glibc */
         #endif
         #include <math.h>
         float x;],
        [x = floorf(x);],
        [FLOORF_LIBM="-lm"])
      LIBS="$save_LIBS"
    fi
    if test "$FLOORF_LIBM" = "?"; then
      FLOORF_LIBM=
    fi
  else
    HAVE_DECL_FLOORF=0
    AC_LIBOBJ([floorf])
    FLOORF_LIBM=
  fi
  AC_SUBST([HAVE_DECL_FLOORF])
  AC_SUBST([FLOORF_LIBM])
])
