# floor.m4 serial 1
dnl Copyright (C) 2007 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_FLOOR],
[
  dnl Test whether floor() can be used without libm.
  FLOOR_LIBM=?
  AC_TRY_LINK([
     #ifndef __NO_MATH_INLINES
     # define __NO_MATH_INLINES 1 /* for glibc */
     #endif
     #include <math.h>
     double x;],
    [x = floor(x);],
    [FLOOR_LIBM=])
  if test "$FLOOR_LIBM" = "?"; then
    save_LIBS="$LIBS"
    LIBS="$LIBS -lm"
    AC_TRY_LINK([
       #ifndef __NO_MATH_INLINES
       # define __NO_MATH_INLINES 1 /* for glibc */
       #endif
       #include <math.h>
       double x;],
      [x = floor(x);],
      [FLOOR_LIBM="-lm"])
    LIBS="$save_LIBS"
  fi
  if test "$FLOOR_LIBM" = "?"; then
    FLOOR_LIBM=
  fi
  AC_SUBST([FLOOR_LIBM])
])
