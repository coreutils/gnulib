# remainderl.m4 serial 2
dnl Copyright (C) 2012 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_REMAINDERL],
[
  m4_divert_text([DEFAULTS], [gl_remainderl_required=plain])
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
    m4_ifdef([gl_FUNC_REMAINDERL_IEEE], [
      if test $gl_remainderl_required = ieee && test $REPLACE_REMAINDERL = 0; then
        AC_CACHE_CHECK([whether remainderl works according to ISO C 99 with IEC 60559],
          [gl_cv_func_remainderl_ieee],
          [
            save_LIBS="$LIBS"
            LIBS="$LIBS $REMAINDERL_LIBM"
            AC_RUN_IFELSE(
              [AC_LANG_SOURCE([[
#ifndef __NO_MATH_INLINES
# define __NO_MATH_INLINES 1 /* for glibc */
#endif
#include <math.h>
/* Compare two numbers with ==.
   This is a separate function because IRIX 6.5 "cc -O" miscompiles an
   'x == x' test.  */
static int
numeric_equal (long double x, long double y)
{
  return x == y;
}
static long double dummy (long double x, long double y) { return 0; }
int main (int argc, char *argv[])
{
  long double (*my_remainderl) (long double, long double) = argc ? remainderl : dummy;
  long double i;
  long double f;
  /* Test remainderl(...,0.0L).
     This test fails on OSF/1 5.1.  */
  f = my_remainderl (2.0L, 0.0L);
  if (numeric_equal (f, f))
    return 1;
  return 0;
}
              ]])],
              [gl_cv_func_remainderl_ieee=yes],
              [gl_cv_func_remainderl_ieee=no],
              [gl_cv_func_remainderl_ieee="guessing no"])
            LIBS="$save_LIBS"
          ])
        case "$gl_cv_func_remainderl_ieee" in
          *yes) ;;
          *) REPLACE_REMAINDERL=1 ;;
        esac
      fi
    ])
  else
    HAVE_REMAINDERL=0
  fi
  if test $HAVE_REMAINDERL = 0 || test $REPLACE_REMAINDERL = 1; then
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
