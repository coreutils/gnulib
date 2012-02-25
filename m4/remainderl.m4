# remainderl.m4 serial 1
dnl Copyright (C) 2012 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_REMAINDERL],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  AC_REQUIRE([gl_LONG_DOUBLE_VS_DOUBLE])
  AC_REQUIRE([gl_FUNC_REMAINDER])

  dnl Test whether remainderl() exists. Assume that remainderl(), if it exists, is
  dnl defined in the same library as remainder().
  save_LIBS="$LIBS"
  LIBS="$LIBS $REMAINDER_LIBM"
  AC_CACHE_CHECK([for remainderl],
    [gl_cv_func_remainderl],
    [
      AC_LINK_IFELSE(
        [AC_LANG_PROGRAM(
           [[#ifndef __NO_MATH_INLINES
             # define __NO_MATH_INLINES 1 /* for glibc */
             #endif
             #include <math.h>
             long double (*funcptr) (long double, long double) = remainderl;
             long double x;
             long double y;]],
           [[return funcptr (x, y) > 1
                    || remainderl (x, y) > 1;]])],
        [gl_cv_func_remainderl=yes],
        [gl_cv_func_remainderl=no])
    ])
  LIBS="$save_LIBS"
  if test $gl_cv_func_remainderl = yes; then
    REMAINDERL_LIBM="$REMAINDER_LIBM"
  else
    HAVE_REMAINDERL=0
    dnl Find libraries needed to link lib/remainderl.c.
    if test $HAVE_SAME_LONG_DOUBLE_AS_DOUBLE = 1; then
      REMAINDERL_LIBM="$REMAINDER_LIBM"
    else
      AC_REQUIRE([gl_FUNC_ROUNDL])
      AC_REQUIRE([gl_FUNC_FMAL])
      REMAINDERL_LIBM=
      dnl Append $ROUNDL_LIBM to REMAINDERL_LIBM, avoiding gratuitous duplicates.
      case " $REMAINDERL_LIBM " in
        *" $ROUNDL_LIBM "*) ;;
        *) REMAINDERL_LIBM="$REMAINDERL_LIBM $ROUNDL_LIBM" ;;
      esac
      dnl Append $FMAL_LIBM to REMAINDERL_LIBM, avoiding gratuitous duplicates.
      case " $REMAINDERL_LIBM " in
        *" $FMAL_LIBM "*) ;;
        *) REMAINDERL_LIBM="$REMAINDERL_LIBM $FMAL_LIBM" ;;
      esac
    fi
  fi
  AC_SUBST([REMAINDERL_LIBM])
])
