# log1pl.m4 serial 3
dnl Copyright (C) 2012-2016 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_LOG1PL],
[
  m4_divert_text([DEFAULTS], [gl_log1pl_required=plain])
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  AC_REQUIRE([gl_LONG_DOUBLE_VS_DOUBLE])
  AC_REQUIRE([gl_FUNC_LOG1P])

  dnl Persuade glibc <math.h> to declare log1pl().
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])

  dnl Test whether log1pl() exists. Assume that log1pl(), if it exists, is
  dnl defined in the same library as log1p().
  save_LIBS="$LIBS"
  LIBS="$LIBS $LOG1P_LIBM"
  AC_CHECK_FUNCS([log1pl])
  LIBS="$save_LIBS"
  if test $ac_cv_func_log1pl = yes; then
    LOG1PL_LIBM="$LOG1P_LIBM"
    m4_ifdef([gl_FUNC_LOG1PL_IEEE], [
      if test $gl_log1pl_required = ieee && test $REPLACE_LOG1PL = 0; then
        AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
        AC_CACHE_CHECK([whether log1pl works according to ISO C 99 with IEC 60559],
          [gl_cv_func_log1pl_ieee],
          [
            save_LIBS="$LIBS"
            LIBS="$LIBS $LOG1PL_LIBM"
            AC_RUN_IFELSE(
              [AC_LANG_SOURCE([[
#ifndef __NO_MATH_INLINES
# define __NO_MATH_INLINES 1 /* for glibc */
#endif
#include <math.h>
]gl_LONG_DOUBLE_MINUS_ZERO_CODE[
]gl_LONG_DOUBLE_SIGNBIT_CODE[
static long double dummy (long double x) { return 0; }
int main (int argc, char *argv[])
{
  long double (*my_log1pl) (long double) = argc ? log1pl : dummy;
  /* This test fails on AIX 7.1, IRIX 6.5.  */
  long double y = my_log1pl (minus_zerol);
  if (!(y == 0.0L) || (signbitl (minus_zerol) && !signbitl (y)))
    return 1;
  return 0;
}
              ]])],
              [gl_cv_func_log1pl_ieee=yes],
              [gl_cv_func_log1pl_ieee=no],
              [case "$host_os" in
                         # Guess yes on glibc systems.
                 *-gnu*) gl_cv_func_log1pl_ieee="guessing yes" ;;
                         # If we don't know, assume the worst.
                 *)      gl_cv_func_log1pl_ieee="guessing no" ;;
               esac
              ])
            LIBS="$save_LIBS"
          ])
        case "$gl_cv_func_log1pl_ieee" in
          *yes) ;;
          *) REPLACE_LOG1PL=1 ;;
        esac
      fi
    ])
  else
    HAVE_LOG1PL=0
    dnl Find libraries needed to link lib/log1pl.c.
    if test $HAVE_SAME_LONG_DOUBLE_AS_DOUBLE = 1; then
      LOG1PL_LIBM="$LOG1P_LIBM"
    else
      AC_REQUIRE([gl_FUNC_ISNANL])
      AC_REQUIRE([gl_FUNC_LOGL])
      AC_REQUIRE([gl_FUNC_ROUNDL])
      LOG1PL_LIBM=
      dnl Append $ISNANL_LIBM to LOG1PL_LIBM, avoiding gratuitous duplicates.
      case " $LOG1PL_LIBM " in
        *" $ISNANL_LIBM "*) ;;
        *) LOG1PL_LIBM="$LOG1PL_LIBM $ISNANL_LIBM" ;;
      esac
      dnl Append $LOGL_LIBM to LOG1PL_LIBM, avoiding gratuitous duplicates.
      case " $LOG1PL_LIBM " in
        *" $LOGL_LIBM "*) ;;
        *) LOG1PL_LIBM="$LOG1PL_LIBM $LOGL_LIBM" ;;
      esac
      dnl Append $ROUNDL_LIBM to LOG1PL_LIBM, avoiding gratuitous duplicates.
      case " $LOG1PL_LIBM " in
        *" $ROUNDL_LIBM "*) ;;
        *) LOG1PL_LIBM="$LOG1PL_LIBM $ROUNDL_LIBM" ;;
      esac
    fi
  fi
  AC_SUBST([LOG1PL_LIBM])
])
