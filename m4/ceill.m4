# ceill.m4 serial 6
dnl Copyright (C) 2007, 2009-2010 Free Software Foundation, Inc.
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
    gl_FUNC_CEILL_LIBS
    if test "$CEILL_LIBM" = "?"; then
      dnl Sun C 5.0 on Solaris declares ceill() and has it in the system-wide
      dnl libm.so, but not in the libm.so that the compiler uses.
      REPLACE_CEILL=1
    fi
  else
    HAVE_DECL_CEILL=0
  fi
  if test $HAVE_DECL_CEILL = 0 || test $REPLACE_CEILL = 1; then
    AC_LIBOBJ([ceill])
    CEILL_LIBM=
  fi
  AC_SUBST([CEILL_LIBM])
])

# Determines the libraries needed to get the ceill() function.
# Sets CEILL_LIBM.
AC_DEFUN([gl_FUNC_CEILL_LIBS],
[
  gl_CACHE_VAL_SILENT([gl_cv_func_ceill_libm], [
    gl_cv_func_ceill_libm=?
    AC_TRY_LINK([
       #ifndef __NO_MATH_INLINES
       # define __NO_MATH_INLINES 1 /* for glibc */
       #endif
       #include <math.h>
       long double x;],
      [x = ceill(x);],
      [gl_cv_func_ceill_libm=])
    if test "$gl_cv_func_ceill_libm" = "?"; then
      save_LIBS="$LIBS"
      LIBS="$LIBS -lm"
      AC_TRY_LINK([
         #ifndef __NO_MATH_INLINES
         # define __NO_MATH_INLINES 1 /* for glibc */
         #endif
         #include <math.h>
         long double x;],
        [x = ceill(x);],
        [gl_cv_func_ceill_libm="-lm"])
      LIBS="$save_LIBS"
    fi
  ])
  CEILL_LIBM="$gl_cv_func_ceill_libm"
])
