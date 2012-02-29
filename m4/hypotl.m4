# hypotl.m4 serial 2
dnl Copyright (C) 2012 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_HYPOTL],
[
  m4_divert_text([DEFAULTS], [gl_hypotl_required=plain])
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  AC_REQUIRE([gl_FUNC_HYPOT])

  dnl Test whether hypotl() exists. Assume that hypotl(), if it exists, is
  dnl defined in the same library as hypot().
  save_LIBS="$LIBS"
  LIBS="$LIBS $HYPOT_LIBM"
  AC_CHECK_FUNCS([hypotl])
  LIBS="$save_LIBS"
  if test $ac_cv_func_hypotl = yes; then
    HYPOTL_LIBM="$HYPOT_LIBM"
    m4_ifdef([gl_FUNC_HYPOTL_IEEE], [
      if test $gl_hypotl_required = ieee && test $REPLACE_HYPOTL = 0; then
        AC_CACHE_CHECK([whether hypotl works according to ISO C 99 with IEC 60559],
          [gl_cv_func_hypotl_ieee],
          [
            save_LIBS="$LIBS"
            LIBS="$LIBS $HYPOTL_LIBM"
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
long double zero;
long double one = 1.0L;
int main (int argc, char *argv[])
{
  long double (*my_hypotl) (long double, long double) = argc ? hypotl : dummy;
  long double f;
  /* Test hypotl(NaN,Infinity).
     This test fails on OSF/1 5.1 and native Windows.  */
  f = my_hypotl (zero / zero, one / zero);
  if (!numeric_equal (f, f))
    return 1;
  return 0;
}
              ]])],
              [gl_cv_func_hypotl_ieee=yes],
              [gl_cv_func_hypotl_ieee=no],
              [gl_cv_func_hypotl_ieee="guessing no"])
            LIBS="$save_LIBS"
          ])
        case "$gl_cv_func_hypotl_ieee" in
          *yes) ;;
          *) REPLACE_HYPOTL=1 ;;
        esac
      fi
    ])
  else
    HAVE_HYPOTL=0
  fi
  if test $HAVE_HYPOTL = 0 || test $REPLACE_HYPOTL = 1; then
    dnl Find libraries needed to link lib/hypotl.c.
    if test $HAVE_SAME_LONG_DOUBLE_AS_DOUBLE = 1; then
      HYPOTL_LIBM="$HYPOT_LIBM"
    else
      AC_REQUIRE([gl_FUNC_FABSL])
      AC_REQUIRE([gl_FUNC_FREXPL])
      AC_REQUIRE([gl_FUNC_LDEXPL])
      AC_REQUIRE([gl_FUNC_SQRTL])
      HYPOTL_LIBM=
      dnl Append $FABSL_LIBM to HYPOTL_LIBM, avoiding gratuitous duplicates.
      case " $HYPOTL_LIBM " in
        *" $FABSL_LIBM "*) ;;
        *) HYPOTL_LIBM="$HYPOTL_LIBM $FABSL_LIBM" ;;
      esac
      dnl Append $FREXPL_LIBM to HYPOTL_LIBM, avoiding gratuitous duplicates.
      case " $HYPOTL_LIBM " in
        *" $FREXPL_LIBM "*) ;;
        *) HYPOTL_LIBM="$HYPOTL_LIBM $FREXPL_LIBM" ;;
      esac
      dnl Append $LDEXPL_LIBM to HYPOTL_LIBM, avoiding gratuitous duplicates.
      case " $HYPOTL_LIBM " in
        *" $LDEXPL_LIBM "*) ;;
        *) HYPOTL_LIBM="$HYPOTL_LIBM $LDEXPL_LIBM" ;;
      esac
      dnl Append $SQRTL_LIBM to HYPOTL_LIBM, avoiding gratuitous duplicates.
      case " $HYPOTL_LIBM " in
        *" $SQRTL_LIBM "*) ;;
        *) HYPOTL_LIBM="$HYPOTL_LIBM $SQRTL_LIBM" ;;
      esac
    fi
  fi
  AC_SUBST([HYPOTL_LIBM])
])
