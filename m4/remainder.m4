# remainder.m4
# serial 14
dnl Copyright (C) 2012-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN([gl_FUNC_REMAINDER],
[
  m4_divert_text([DEFAULTS], [gl_remainder_required=plain])
  AC_REQUIRE([gl_MATH_H_DEFAULTS])

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
        saved_LIBS="$LIBS"
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
        LIBS="$saved_LIBS"
      ])
    if test $gl_cv_func_remainder_in_libm = yes; then
      REMAINDER_LIBM=-lm
    fi
  fi
  if test $gl_cv_func_remainder_no_libm = yes \
     || test $gl_cv_func_remainder_in_libm = yes; then
    :
  else
    HAVE_REMAINDER=0
  fi
  if test $HAVE_REMAINDER = 0 || test $REPLACE_REMAINDER = 1; then
    dnl Find libraries needed to link lib/remainder.c.
    AC_REQUIRE([gl_FUNC_FABS])
    AC_REQUIRE([gl_FUNC_FMOD])
    AC_REQUIRE([gl_FUNC_ISNAND])
    REMAINDER_LIBM=
    dnl Append $FABS_LIBM to REMAINDER_LIBM, avoiding gratuitous duplicates.
    case " $REMAINDER_LIBM " in
      *" $FABS_LIBM "*) ;;
      *) REMAINDER_LIBM="$REMAINDER_LIBM $FABS_LIBM" ;;
    esac
    dnl Append $FMOD_LIBM to REMAINDER_LIBM, avoiding gratuitous duplicates.
    case " $REMAINDER_LIBM " in
      *" $FMOD_LIBM "*) ;;
      *) REMAINDER_LIBM="$REMAINDER_LIBM $FMOD_LIBM" ;;
    esac
    dnl Append $ISNAND_LIBM to REMAINDER_LIBM, avoiding gratuitous duplicates.
    case " $REMAINDER_LIBM " in
      *" $ISNAND_LIBM "*) ;;
      *) REMAINDER_LIBM="$REMAINDER_LIBM $ISNAND_LIBM" ;;
    esac
  fi
  AC_SUBST([REMAINDER_LIBM])
])
