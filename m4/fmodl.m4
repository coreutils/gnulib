# fmodl.m4 serial 2
dnl Copyright (C) 2011-2012 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_FMODL],
[
  m4_divert_text([DEFAULTS], [gl_fmodl_required=plain])
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  AC_REQUIRE([gl_LONG_DOUBLE_VS_DOUBLE])
  AC_REQUIRE([gl_FUNC_FMOD])

  dnl Test whether fmodl() is declared.
  dnl AIX 5.1 has fmodl() in libc but doesn't declare it in <math.h>, and the
  dnl function produces NaN results for finite values.
  AC_CHECK_DECL([fmodl],
    [ac_cv_have_decl_fmodl=yes], [ac_cv_have_decl_fmodl=no],
    [[#include <math.h>]])

  dnl Test whether fmodl() exists. Assume that fmodl(), if it exists, is
  dnl defined in the same library as fmod().
  save_LIBS="$LIBS"
  LIBS="$LIBS $FMOD_LIBM"
  AC_CHECK_FUNCS([fmodl])
  LIBS="$save_LIBS"
  if test $ac_cv_func_fmodl = yes; then
    FMODL_LIBM="$FMOD_LIBM"
    if test $ac_cv_have_decl_fmodl = no; then
      REPLACE_FMODL=1
    fi
    m4_ifdef([gl_FUNC_FMODL_IEEE], [
      if test $gl_fmodl_required = ieee && test $REPLACE_FMODL = 0; then
        AC_CACHE_CHECK([whether fmodl works according to ISO C 99 with IEC 60559],
          [gl_cv_func_fmodl_ieee],
          [
            save_LIBS="$LIBS"
            LIBS="$LIBS $FMODL_LIBM"
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
  long double (*my_fmodl) (long double, long double) = argc ? fmodl : dummy;
  long double i;
  long double f;
  /* Test fmodl(...,0.0L).
     This test fails on OSF/1 5.1.  */
  f = my_fmodl (2.0L, 0.0L);
  if (numeric_equal (f, f))
    return 1;
  return 0;
}
              ]])],
              [gl_cv_func_fmodl_ieee=yes],
              [gl_cv_func_fmodl_ieee=no],
              [gl_cv_func_fmodl_ieee="guessing no"])
            LIBS="$save_LIBS"
          ])
        case "$gl_cv_func_fmodl_ieee" in
          *yes) ;;
          *) REPLACE_FMODL=1 ;;
        esac
      fi
    ])
  else
    HAVE_FMODL=0
  fi
  if test $HAVE_FMODL = 0 || test $REPLACE_FMODL = 1; then
    dnl Find libraries needed to link lib/fmodl.c.
    if test $HAVE_SAME_LONG_DOUBLE_AS_DOUBLE = 1; then
      FMODL_LIBM="$FMOD_LIBM"
    else
      AC_REQUIRE([gl_FUNC_TRUNCL])
      AC_REQUIRE([gl_FUNC_FMAL])
      FMODL_LIBM=
      dnl Append $TRUNCL_LIBM to FMODL_LIBM, avoiding gratuitous duplicates.
      case " $FMODL_LIBM " in
        *" $TRUNCL_LIBM "*) ;;
        *) FMODL_LIBM="$FMODL_LIBM $TRUNCL_LIBM" ;;
      esac
      dnl Append $FMAL_LIBM to FMODL_LIBM, avoiding gratuitous duplicates.
      case " $FMODL_LIBM " in
        *" $FMAL_LIBM "*) ;;
        *) FMODL_LIBM="$FMODL_LIBM $FMAL_LIBM" ;;
      esac
    fi
  fi
  AC_SUBST([FMODL_LIBM])
])
