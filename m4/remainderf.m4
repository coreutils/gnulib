# remainderf.m4 serial 1
dnl Copyright (C) 2012 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_REMAINDERF],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  AC_REQUIRE([gl_FUNC_REMAINDER])

  dnl Test whether remainderf() exists. Assume that remainderf(), if it exists, is
  dnl defined in the same library as remainder().
  save_LIBS="$LIBS"
  LIBS="$LIBS $REMAINDER_LIBM"
  AC_CACHE_CHECK([for remainderf],
    [gl_cv_func_remainderf],
    [
      AC_LINK_IFELSE(
        [AC_LANG_PROGRAM(
           [[#ifndef __NO_MATH_INLINES
             # define __NO_MATH_INLINES 1 /* for glibc */
             #endif
             #include <math.h>
             float (*funcptr) (float, float) = remainderf;
             float x;
             float y;]],
           [[return funcptr (x, y) > 1
                    || remainderf (x, y) > 1;]])],
        [gl_cv_func_remainderf=yes],
        [gl_cv_func_remainderf=no])
    ])
  LIBS="$save_LIBS"
  if test $gl_cv_func_remainderf = yes; then
    REMAINDERF_LIBM="$REMAINDER_LIBM"
  else
    HAVE_REMAINDERF=0
    dnl Find libraries needed to link lib/remainderf.c.
    if test $gl_cv_func_remainder_no_libm = yes \
       || test $gl_cv_func_remainder_in_libm = yes; then
      AC_DEFINE([HAVE_REMAINDER], [1],
        [Define to 1 if the remainder() function is available in libc or libm.])
      REMAINDERF_LIBM="$REMAINDER_LIBM"
    else
      AC_REQUIRE([gl_FUNC_ROUNDF])
      AC_REQUIRE([gl_FUNC_FMAF])
      REMAINDERF_LIBM=
      dnl Append $ROUNDF_LIBM to REMAINDERF_LIBM, avoiding gratuitous duplicates.
      case " $REMAINDERF_LIBM " in
        *" $ROUNDF_LIBM "*) ;;
        *) REMAINDERF_LIBM="$REMAINDERF_LIBM $ROUNDF_LIBM" ;;
      esac
      dnl Append $FMAF_LIBM to REMAINDERF_LIBM, avoiding gratuitous duplicates.
      case " $REMAINDERF_LIBM " in
        *" $FMAF_LIBM "*) ;;
        *) REMAINDERF_LIBM="$REMAINDERF_LIBM $FMAF_LIBM" ;;
      esac
    fi
  fi
  AC_SUBST([REMAINDERF_LIBM])
])
