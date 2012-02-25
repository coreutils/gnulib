# remainder.m4 serial 1
dnl Copyright (C) 2012 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_REMAINDER],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])

  dnl Test whether remainder() is declared. On IRIX 5.3 it is not declared.
  AC_CHECK_DECL([remainder], , [HAVE_DECL_REMAINDER=0], [[#include <math.h>]])

  REMAINDER_LIBM=
  AC_CACHE_CHECK([whether remainder() can be used without linking with libm],
    [gl_cv_func_remainder_no_libm],
    [
      AC_LINK_IFELSE(
        [AC_LANG_PROGRAM(
           [[#ifndef __NO_MATH_INLINES
             # define __NO_MATH_INLINES 1 /* for glibc */
             #endif
             #include <math.h>
             double x;
             double y;]],
           [[return remainder (x, y) > 1;]])],
        [gl_cv_func_remainder_no_libm=yes],
        [gl_cv_func_remainder_no_libm=no])
    ])
  if test $gl_cv_func_remainder_no_libm = no; then
    AC_CACHE_CHECK([whether remainder() can be used with libm],
      [gl_cv_func_remainder_in_libm],
      [
        save_LIBS="$LIBS"
        LIBS="$LIBS -lm"
        AC_LINK_IFELSE(
          [AC_LANG_PROGRAM(
             [[#ifndef __NO_MATH_INLINES
               # define __NO_MATH_INLINES 1 /* for glibc */
               #endif
               #include <math.h>
               double x;
               double y;]],
             [[return remainder (x, y) > 1;]])],
          [gl_cv_func_remainder_in_libm=yes],
          [gl_cv_func_remainder_in_libm=no])
        LIBS="$save_LIBS"
      ])
    if test $gl_cv_func_remainder_in_libm = yes; then
      REMAINDER_LIBM=-lm
    fi
  fi
  if test $gl_cv_func_remainder_no_libm = no \
     && test $gl_cv_func_remainder_in_libm = no; then
    HAVE_REMAINDER=0
    dnl Find libraries needed to link lib/remainder.c.
    AC_REQUIRE([gl_FUNC_ROUND])
    AC_REQUIRE([gl_FUNC_FMA])
    REMAINDER_LIBM=
    dnl Append $ROUND_LIBM to REMAINDER_LIBM, avoiding gratuitous duplicates.
    case " $REMAINDER_LIBM " in
      *" $ROUND_LIBM "*) ;;
      *) REMAINDER_LIBM="$REMAINDER_LIBM $ROUND_LIBM" ;;
    esac
    dnl Append $FMA_LIBM to REMAINDER_LIBM, avoiding gratuitous duplicates.
    case " $REMAINDER_LIBM " in
      *" $FMA_LIBM "*) ;;
      *) REMAINDER_LIBM="$REMAINDER_LIBM $FMA_LIBM" ;;
    esac
  fi
  AC_SUBST([REMAINDER_LIBM])
])
