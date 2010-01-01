# ceil.m4 serial 3
dnl Copyright (C) 2007, 2009, 2010 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_CEIL],
[
  dnl Test whether ceil() can be used without libm.
  gl_FUNC_CEIL_LIBS
  if test "$CEIL_LIBM" = "?"; then
    CEIL_LIBM=
  fi
  AC_SUBST([CEIL_LIBM])
])

# Determines the libraries needed to get the ceil() function.
# Sets CEIL_LIBM.
AC_DEFUN([gl_FUNC_CEIL_LIBS],
[
  gl_CACHE_VAL_SILENT([gl_cv_func_ceil_libm], [
    gl_cv_func_ceil_libm=?
    AC_TRY_LINK([
       #ifndef __NO_MATH_INLINES
       # define __NO_MATH_INLINES 1 /* for glibc */
       #endif
       #include <math.h>
       double x;],
      [x = ceil(x);],
      [gl_cv_func_ceil_libm=])
    if test "$gl_cv_func_ceil_libm" = "?"; then
      save_LIBS="$LIBS"
      LIBS="$LIBS -lm"
      AC_TRY_LINK([
         #ifndef __NO_MATH_INLINES
         # define __NO_MATH_INLINES 1 /* for glibc */
         #endif
         #include <math.h>
         double x;],
        [x = ceil(x);],
        [gl_cv_func_ceil_libm="-lm"])
      LIBS="$save_LIBS"
    fi
  ])
  CEIL_LIBM="$gl_cv_func_ceil_libm"
])
