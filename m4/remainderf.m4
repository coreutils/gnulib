# remainderf.m4
# serial 18
dnl Copyright (C) 2012-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN([gl_FUNC_REMAINDERF],
[
  m4_divert_text([DEFAULTS], [gl_remainderf_required=plain])
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  AC_REQUIRE([gl_FUNC_REMAINDER])

  dnl Persuade glibc <math.h> to declare remainderf().
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])

  dnl Test whether remainderf() exists. Assume that remainderf(), if it exists, is
  dnl defined in the same library as remainder().
  saved_LIBS="$LIBS"
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
             extern
             #ifdef __cplusplus
             "C"
             #endif
             float remainderf (float, float);
             float (*funcptr) (float, float) = remainderf;
             float x;
             float y;]],
           [[return funcptr (x, y) > 1
                    || remainderf (x, y) > 1;]])],
        [gl_cv_func_remainderf=yes],
        [gl_cv_func_remainderf=no])
    ])
  LIBS="$saved_LIBS"
  if test $gl_cv_func_remainderf = yes; then
    REMAINDERF_LIBM="$REMAINDER_LIBM"
  else
    HAVE_REMAINDERF=0
  fi
  if test $HAVE_REMAINDERF = 0; then
    dnl Find libraries needed to link lib/remainderf.c.
    if test $gl_cv_func_remainder_no_libm = yes \
       || test $gl_cv_func_remainder_in_libm = yes; then
      AC_DEFINE([HAVE_REMAINDER], [1],
        [Define to 1 if the remainder() function is available in libc or libm.])
      REMAINDERF_LIBM="$REMAINDER_LIBM"
    else
      AC_REQUIRE([gl_FUNC_FABSF])
      AC_REQUIRE([gl_FUNC_FMODF])
      AC_REQUIRE([gl_FUNC_ISNANF])
      REMAINDERF_LIBM=
      dnl Append $FABSF_LIBM to REMAINDERF_LIBM, avoiding gratuitous duplicates.
      case " $REMAINDERF_LIBM " in
        *" $FABSF_LIBM "*) ;;
        *) REMAINDERF_LIBM="$REMAINDERF_LIBM $FABSF_LIBM" ;;
      esac
      dnl Append $FMODF_LIBM to REMAINDERF_LIBM, avoiding gratuitous duplicates.
      case " $REMAINDERF_LIBM " in
        *" $FMODF_LIBM "*) ;;
        *) REMAINDERF_LIBM="$REMAINDERF_LIBM $FMODF_LIBM" ;;
      esac
      dnl Append $ISNANF_LIBM to REMAINDERF_LIBM, avoiding gratuitous duplicates.
      case " $REMAINDERF_LIBM " in
        *" $ISNANF_LIBM "*) ;;
        *) REMAINDERF_LIBM="$REMAINDERF_LIBM $ISNANF_LIBM" ;;
      esac
    fi
  fi
  AC_SUBST([REMAINDERF_LIBM])
])
