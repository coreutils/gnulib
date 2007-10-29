# floorl.m4 serial 2
dnl Copyright (C) 2007 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_FLOORL],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  dnl Persuade glibc <math.h> to declare floorl().
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])
  dnl Test whether floorl() is declared.
  AC_CHECK_DECLS([floorl], , , [#include <math.h>])
  if test "$ac_cv_have_decl_floorl" = yes; then
    dnl Test whether floorl() can be used without libm.
    gl_FUNC_FLOORL_LIBS
    if test "$FLOORL_LIBM" = "?"; then
      FLOORL_LIBM=
    fi
  else
    HAVE_DECL_FLOORL=0
    AC_LIBOBJ([floorl])
    FLOORL_LIBM=
  fi
  AC_SUBST([HAVE_DECL_FLOORL])
  AC_SUBST([FLOORL_LIBM])
])

# Determines the libraries needed to get the floorl() function.
# Sets FLOORL_LIBM.
AC_DEFUN([gl_FUNC_FLOORL_LIBS],
[
  AC_CACHE_VAL([gl_func_floorl_libm], [
    gl_func_floorl_libm=?
    AC_TRY_LINK([
       #ifndef __NO_MATH_INLINES
       # define __NO_MATH_INLINES 1 /* for glibc */
       #endif
       #include <math.h>
       long double x;],
      [x = floorl(x);],
      [gl_func_floorl_libm=])
    if test "$gl_func_floorl_libm" = "?"; then
      save_LIBS="$LIBS"
      LIBS="$LIBS -lm"
      AC_TRY_LINK([
         #ifndef __NO_MATH_INLINES
         # define __NO_MATH_INLINES 1 /* for glibc */
         #endif
         #include <math.h>
         long double x;],
        [x = floorl(x);],
        [gl_func_floorl_libm="-lm"])
      LIBS="$save_LIBS"
    fi
  ])
  FLOORL_LIBM="$gl_func_floorl_libm"
])
