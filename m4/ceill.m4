# ceill.m4 serial 1
dnl Copyright (C) 2007 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_CEILL],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  dnl Persuade glibc <math.h> to declare ceill().
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])
  dnl Test whether ceill() is declared.
  AC_CHECK_DECLS([ceill], , , [#include <math.h>])
  if test "$ac_cv_have_decl_ceill" = yes; then
    dnl Test whether ceill() can be used without libm.
    CEILL_LIBM=?
    AC_TRY_LINK([
       #ifndef __NO_MATH_INLINES
       # define __NO_MATH_INLINES 1 /* for glibc */
       #endif
       #include <math.h>
       long double x;],
      [x = ceill(x);],
      [CEILL_LIBM=])
    if test "$CEILL_LIBM" = "?"; then
      save_LIBS="$LIBS"
      LIBS="$LIBS -lm"
      AC_TRY_LINK([
         #ifndef __NO_MATH_INLINES
         # define __NO_MATH_INLINES 1 /* for glibc */
         #endif
         #include <math.h>
         long double x;],
        [x = ceill(x);],
        [CEILL_LIBM="-lm"])
      LIBS="$save_LIBS"
    fi
    if test "$CEILL_LIBM" = "?"; then
      CEILL_LIBM=
    fi
  else
    HAVE_DECL_CEILL=0
    AC_LIBOBJ([ceill])
    CEILL_LIBM=
  fi
  AC_SUBST([HAVE_DECL_CEILL])
  AC_SUBST([CEILL_LIBM])
])
