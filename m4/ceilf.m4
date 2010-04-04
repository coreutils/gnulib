# ceilf.m4 serial 6
dnl Copyright (C) 2007, 2009-2010 Free Software Foundation, Inc.
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
    gl_FUNC_CEILF_LIBS
    if test "$CEILF_LIBM" = "?"; then
      dnl Sun C 5.0 on Solaris declares ceilf() and has it in the system-wide
      dnl libm.so, but not in the libm.so that the compiler uses.
      REPLACE_CEILF=1
    fi
  else
    HAVE_DECL_CEILF=0
  fi
  if test $HAVE_DECL_CEILF = 0 || test $REPLACE_CEILF = 1; then
    AC_LIBOBJ([ceilf])
    CEILF_LIBM=
  fi
  AC_SUBST([CEILF_LIBM])
])

# Determines the libraries needed to get the ceilf() function.
# Sets CEILF_LIBM.
AC_DEFUN([gl_FUNC_CEILF_LIBS],
[
  gl_CACHE_VAL_SILENT([gl_cv_func_ceilf_libm], [
    gl_cv_func_ceilf_libm=?
    AC_TRY_LINK([
       #ifndef __NO_MATH_INLINES
       # define __NO_MATH_INLINES 1 /* for glibc */
       #endif
       #include <math.h>
       float x;],
      [x = ceilf(x);],
      [gl_cv_func_ceilf_libm=])
    if test "$gl_cv_func_ceilf_libm" = "?"; then
      save_LIBS="$LIBS"
      LIBS="$LIBS -lm"
      AC_TRY_LINK([
         #ifndef __NO_MATH_INLINES
         # define __NO_MATH_INLINES 1 /* for glibc */
         #endif
         #include <math.h>
         float x;],
        [x = ceilf(x);],
        [gl_cv_func_ceilf_libm="-lm"])
      LIBS="$save_LIBS"
    fi
  ])
  CEILF_LIBM="$gl_cv_func_ceilf_libm"
])
