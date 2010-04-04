# roundf.m4 serial 9
dnl Copyright (C) 2007-2010 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_ROUNDF],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  dnl Persuade glibc <math.h> to declare roundf().
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])
  AC_CHECK_DECLS([roundf], , , [#include <math.h>])
  if test "$ac_cv_have_decl_roundf" = yes; then
    gl_CHECK_MATH_LIB([ROUNDF_LIBM], [x = roundf (x);])
    if test "$ROUNDF_LIBM" != missing; then
      dnl Test whether roundf() produces correct results. On mingw, for
      dnl x = 1/2 - 2^-25, the system's roundf() returns a wrong result.
      AC_REQUIRE([AC_PROG_CC])
      AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
      AC_CACHE_CHECK([whether roundf works], [gl_cv_func_roundf_works],
        [
          save_LIBS="$LIBS"
          LIBS="$LIBS $ROUNDF_LIBM"
          AC_RUN_IFELSE([AC_LANG_SOURCE([[
#include <float.h>
#include <math.h>
int main()
{
  /* 2^FLT_MANT_DIG.  */
  static const float TWO_MANT_DIG =
    /* Assume FLT_MANT_DIG <= 3 * 31.
       Use the identity  n = floor(n/3) + floor((n+1)/3) + floor((n+2)/3).  */
    (float) (1U << (FLT_MANT_DIG / 3))
    * (float) (1U << ((FLT_MANT_DIG + 1) / 3))
    * (float) (1U << ((FLT_MANT_DIG + 2) / 3));
  volatile float x = 0.5f - 0.5f / TWO_MANT_DIG;
  exit (x < 0.5f && roundf (x) != 0.0f);
}]])], [gl_cv_func_roundf_works=yes], [gl_cv_func_roundf_works=no],
          [case "$host_os" in
             mingw*) gl_cv_func_roundf_works="guessing no";;
             *)      gl_cv_func_roundf_works="guessing yes";;
           esac
          ])
          LIBS="$save_LIBS"
        ])
      case "$gl_cv_func_roundf_works" in
        *no) ROUNDF_LIBM=missing ;;
      esac
    fi
    if test "$ROUNDF_LIBM" = missing; then
      REPLACE_ROUNDF=1
    fi
  else
    HAVE_DECL_ROUNDF=0
  fi
  if test $HAVE_DECL_ROUNDF = 0 || test $REPLACE_ROUNDF = 1; then
    AC_LIBOBJ([roundf])
    AC_CHECK_DECLS([ceilf, floorf], , , [#include <math.h>])
    if test "$ac_cv_have_decl_floorf" = yes \
       && test "$ac_cv_have_decl_ceilf" = yes; then
      gl_FUNC_FLOORF_LIBS
      gl_FUNC_CEILF_LIBS
      if test "$FLOORF_LIBM" != '?' && test "$CEILF_LIBM" != '?'; then
        AC_DEFINE([HAVE_FLOORF_AND_CEILF], [1],
          [Define if the both the floorf() and ceilf() functions exist.])
        ROUNDF_LIBM=
        dnl Append $FLOORF_LIBM to ROUNDF_LIBM, avoiding gratuitous duplicates.
        case " $ROUNDF_LIBM " in
          *" $FLOORF_LIBM "*) ;;
          *) ROUNDF_LIBM="$ROUNDF_LIBM $FLOORF_LIBM" ;;
        esac
        dnl Append $CEILF_LIBM to ROUNDF_LIBM, avoiding gratuitous duplicates.
        case " $ROUNDF_LIBM " in
          *" $CEILF_LIBM "*) ;;
          *) ROUNDF_LIBM="$ROUNDF_LIBM $CEILF_LIBM" ;;
        esac
      else
        ROUNDF_LIBM=
      fi
    else
      ROUNDF_LIBM=
    fi
  fi
  AC_SUBST([ROUNDF_LIBM])
])
