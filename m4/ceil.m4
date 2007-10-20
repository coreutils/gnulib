# ceil.m4 serial 1
dnl Copyright (C) 2007 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_CEIL],
[
  dnl Test whether ceil() can be used without libm.
  CEIL_LIBM=?
  AC_TRY_LINK([
     #ifndef __NO_MATH_INLINES
     # define __NO_MATH_INLINES 1 /* for glibc */
     #endif
     #include <math.h>
     double x;],
    [x = ceil(x);],
    [CEIL_LIBM=])
  if test "$CEIL_LIBM" = "?"; then
    save_LIBS="$LIBS"
    LIBS="$LIBS -lm"
    AC_TRY_LINK([
       #ifndef __NO_MATH_INLINES
       # define __NO_MATH_INLINES 1 /* for glibc */
       #endif
       #include <math.h>
       double x;],
      [x = ceil(x);],
      [CEIL_LIBM="-lm"])
    LIBS="$save_LIBS"
  fi
  if test "$CEIL_LIBM" = "?"; then
    CEIL_LIBM=
  fi
  AC_SUBST([CEIL_LIBM])
])
