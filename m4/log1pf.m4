# log1pf.m4
# serial 12
dnl Copyright (C) 2012-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN([gl_FUNC_LOG1PF],
[
  m4_divert_text([DEFAULTS], [gl_log1pf_required=plain])
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  AC_REQUIRE([gl_FUNC_LOG1P])

  dnl Persuade glibc <math.h> to declare log1pf().
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])

  dnl Test whether log1pf() exists. Assume that log1pf(), if it exists, is
  dnl defined in the same library as log1p().
  saved_LIBS="$LIBS"
  LIBS="$LIBS $LOG1P_LIBM"
  AC_CHECK_FUNCS([log1pf])
  LIBS="$saved_LIBS"
  if test $ac_cv_func_log1pf = yes; then
    LOG1PF_LIBM="$LOG1P_LIBM"

    m4_ifdef([gl_FUNC_LOG1PF_IEEE], [
      if test $gl_log1pf_required = ieee; then
        AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
        AC_CACHE_CHECK([whether log1pf works according to ISO C 99 with IEC 60559],
          [gl_cv_func_log1pf_ieee],
          [
            saved_LIBS="$LIBS"
            LIBS="$LIBS $LOG1PF_LIBM"
            AC_RUN_IFELSE(
              [AC_LANG_SOURCE([[
#ifndef __NO_MATH_INLINES
# define __NO_MATH_INLINES 1 /* for glibc */
#endif
#include <math.h>
]gl_FLOAT_MINUS_ZERO_CODE[
]gl_FLOAT_SIGNBIT_CODE[
static float dummy (float x) { return 0; }
int main (int argc, char *argv[])
{
  float (* volatile my_log1pf) (float) = argc ? log1pf : dummy;
  /* This test fails on OpenBSD 4.9, AIX 7.1.  */
  float y = my_log1pf (minus_zerof);
  if (!(y == 0.0f) || (signbitf (minus_zerof) && !signbitf (y)))
    return 1;
  return 0;
}
              ]])],
              [gl_cv_func_log1pf_ieee=yes],
              [gl_cv_func_log1pf_ieee=no],
              [case "$host_os" in
                                     # Guess yes on glibc systems.
                 *-gnu* | gnu*)      gl_cv_func_log1pf_ieee="guessing yes" ;;
                                     # Guess yes on musl systems.
                 *-musl* | midipix*) gl_cv_func_log1pf_ieee="guessing yes" ;;
                                     # Guess yes on native Windows.
                 mingw* | windows*)  gl_cv_func_log1pf_ieee="guessing yes" ;;
                                     # If we don't know, obey --enable-cross-guesses.
                 *)                  gl_cv_func_log1pf_ieee="$gl_cross_guess_normal" ;;
               esac
              ])
            LIBS="$saved_LIBS"
          ])
        case "$gl_cv_func_log1pf_ieee" in
          *yes) ;;
          *) REPLACE_LOG1PF=1 ;;
        esac
      fi
    ])
  else
    HAVE_LOG1PF=0
  fi
  if test $HAVE_LOG1PF = 0 || test $REPLACE_LOG1PF = 1; then
    dnl Find libraries needed to link lib/log1pf.c.
    LOG1PF_LIBM="$LOG1P_LIBM"
  fi
  AC_SUBST([LOG1PF_LIBM])
])
