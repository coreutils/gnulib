# round.m4 serial 8
dnl Copyright (C) 2007, 2009-2010 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_ROUND],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  dnl Persuade glibc <math.h> to declare round().
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])
  AC_CHECK_DECLS([round], , , [#include <math.h>])
  if test "$ac_cv_have_decl_round" = yes; then
    gl_CHECK_MATH_LIB([ROUND_LIBM], [x = round (x);])
    if test "$ROUND_LIBM" != missing; then
      dnl Test whether round() produces correct results. On NetBSD 3.0, for
      dnl x = 1/2 - 2^-54, the system's round() returns a wrong result.
      AC_REQUIRE([AC_PROG_CC])
      AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
      AC_CACHE_CHECK([whether round works], [gl_cv_func_round_works],
        [
          save_LIBS="$LIBS"
          LIBS="$LIBS $ROUND_LIBM"
          AC_RUN_IFELSE([AC_LANG_SOURCE([[
#include <float.h>
#include <math.h>
int main()
{
  /* 2^DBL_MANT_DIG.  */
  static const double TWO_MANT_DIG =
    /* Assume DBL_MANT_DIG <= 5 * 31.
       Use the identity
       n = floor(n/5) + floor((n+1)/5) + ... + floor((n+4)/5).  */
    (double) (1U << (DBL_MANT_DIG / 5))
    * (double) (1U << ((DBL_MANT_DIG + 1) / 5))
    * (double) (1U << ((DBL_MANT_DIG + 2) / 5))
    * (double) (1U << ((DBL_MANT_DIG + 3) / 5))
    * (double) (1U << ((DBL_MANT_DIG + 4) / 5));
  volatile double x = 0.5 - 0.5 / TWO_MANT_DIG;
  exit (x < 0.5 && round (x) != 0.0);
}]])], [gl_cv_func_round_works=yes], [gl_cv_func_round_works=no],
          [case "$host_os" in
             netbsd*) gl_cv_func_round_works="guessing no";;
             *)       gl_cv_func_round_works="guessing yes";;
           esac
          ])
          LIBS="$save_LIBS"
        ])
      case "$gl_cv_func_round_works" in
        *no) ROUND_LIBM=missing ;;
      esac
    fi
    if test "$ROUND_LIBM" = missing; then
      REPLACE_ROUND=1
    fi
  else
    HAVE_DECL_ROUND=0
  fi
  if test $HAVE_DECL_ROUND = 0 || test $REPLACE_ROUND = 1; then
    AC_LIBOBJ([round])
    gl_FUNC_FLOOR_LIBS
    gl_FUNC_CEIL_LIBS
    ROUND_LIBM=
    dnl Append $FLOOR_LIBM to ROUND_LIBM, avoiding gratuitous duplicates.
    case " $ROUND_LIBM " in
      *" $FLOOR_LIBM "*) ;;
      *) ROUND_LIBM="$ROUND_LIBM $FLOOR_LIBM" ;;
    esac
    dnl Append $CEIL_LIBM to ROUND_LIBM, avoiding gratuitous duplicates.
    case " $ROUND_LIBM " in
      *" $CEIL_LIBM "*) ;;
      *) ROUND_LIBM="$ROUND_LIBM $CEIL_LIBM" ;;
    esac
  fi
  AC_SUBST([ROUND_LIBM])
])
