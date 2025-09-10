# expm1f.m4
# serial 8
dnl Copyright (C) 2011-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN([gl_FUNC_EXPM1F],
[
  m4_divert_text([DEFAULTS], [gl_expm1f_required=plain])
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  AC_REQUIRE([gl_FUNC_EXPM1])

  dnl Persuade glibc <math.h> to declare expm1f().
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])

  dnl Test whether expm1f() exists. Assume that expm1f(), if it exists, is
  dnl defined in the same library as expm1().
  saved_LIBS="$LIBS"
  LIBS="$LIBS $EXPM1_LIBM"
  AC_CHECK_FUNCS([expm1f])
  LIBS="$saved_LIBS"
  if test $ac_cv_func_expm1f = yes; then
    EXPM1F_LIBM="$EXPM1_LIBM"
    m4_ifdef([gl_FUNC_EXPM1F_IEEE], [
      if test $gl_expm1f_required = ieee; then
        AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
        AC_CACHE_CHECK([whether expm1f works according to ISO C 99 with IEC 60559],
          [gl_cv_func_expm1f_ieee],
          [
            saved_LIBS="$LIBS"
            LIBS="$LIBS $EXPM1F_LIBM"
            AC_RUN_IFELSE(
              [AC_LANG_SOURCE([[
#ifndef __NO_MATH_INLINES
# define __NO_MATH_INLINES 1 /* for glibc */
#endif
#include <math.h>
]gl_FLOAT_MINUS_ZERO_CODE[
]gl_FLOAT_SIGNBIT_CODE[
#ifndef expm1f
extern
#ifdef __cplusplus
"C"
#endif
float expm1f (float);
#endif
/* Compare two numbers with ==.
   This is a separate function in order to disable compiler optimizations.  */
static int
numeric_equal (float x, float y)
{
  return x == y;
}
static float dummy (float x) { return 0; }
int main (int argc, char *argv[])
{
  float (* volatile my_expm1f) (float) = argc ? expm1f : dummy;
  /* Test expm1f(-0.0f).
     This test fails on AIX 7.2.  */
  float y = my_expm1f (minus_zerof);
  if (signbitf (minus_zerof) && !signbitf (y))
    return 1;
  return 0;
}
              ]])],
              [gl_cv_func_expm1f_ieee=yes],
              [gl_cv_func_expm1f_ieee=no],
              [case "$host_os" in
                                     # Guess yes on glibc systems.
                 *-gnu* | gnu*)      gl_cv_func_expm1f_ieee="guessing yes" ;;
                                     # Guess yes on musl systems.
                 *-musl* | midipix*) gl_cv_func_expm1f_ieee="guessing yes" ;;
                                     # Guess yes on native Windows.
                 mingw* | windows*)  gl_cv_func_expm1f_ieee="guessing yes" ;;
                                     # Guess no on AIX.
                 aix*)               gl_cv_func_expm1f_ieee="guessing no" ;;
                                     # If we don't know, obey --enable-cross-guesses.
                 *)                  gl_cv_func_expm1f_ieee="$gl_cross_guess_normal" ;;
               esac
              ])
            LIBS="$saved_LIBS"
          ])
        case "$gl_cv_func_expm1f_ieee" in
          *yes) ;;
          *) REPLACE_EXPM1F=1 ;;
        esac
      fi
    ])
  else
    HAVE_EXPM1F=0
  fi
  if test $HAVE_EXPM1F = 0 || test $REPLACE_EXPM1F = 1; then
    dnl Find libraries needed to link lib/expm1f.c.
    EXPM1F_LIBM="$EXPM1_LIBM"
  fi
  AC_SUBST([EXPM1F_LIBM])
])
