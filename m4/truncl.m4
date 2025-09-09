# truncl.m4
# serial 21
dnl Copyright (C) 2007-2008, 2010-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN([gl_FUNC_TRUNCL],
[
  m4_divert_text([DEFAULTS], [gl_truncl_required=plain])
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  AC_REQUIRE([gl_LONG_DOUBLE_VS_DOUBLE])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles

  dnl Persuade glibc <math.h> to declare truncl().
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])

  dnl Test whether truncl() is declared.
  AC_CHECK_DECLS([truncl], , , [[#include <math.h>]])
  if test "$ac_cv_have_decl_truncl" = yes; then
    dnl Test whether truncl() can be used without libm.
    TRUNCL_LIBM=?
    AC_LINK_IFELSE(
      [AC_LANG_PROGRAM(
         [[#ifndef __NO_MATH_INLINES
           # define __NO_MATH_INLINES 1 /* for glibc */
           #endif
           #include <math.h>
           long double (*funcptr) (long double) = truncl;
           long double x;]],
         [[x = funcptr(x) + truncl(x);]])],
      [TRUNCL_LIBM=])
    if test "$TRUNCL_LIBM" = "?"; then
      saved_LIBS="$LIBS"
      LIBS="$LIBS -lm"
      AC_LINK_IFELSE(
        [AC_LANG_PROGRAM(
           [[#ifndef __NO_MATH_INLINES
             # define __NO_MATH_INLINES 1 /* for glibc */
             #endif
             #include <math.h>
             long double (*funcptr) (long double) = truncl;
             long double x;]],
           [[x = funcptr(x) + truncl(x);]])],
        [TRUNCL_LIBM="-lm"])
      LIBS="$saved_LIBS"
    fi
    if test "$TRUNCL_LIBM" = "?"; then
      TRUNCL_LIBM=
    fi
    m4_ifdef([gl_FUNC_TRUNCL_IEEE], [
      if test $gl_truncl_required = ieee; then
        AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
        AC_CACHE_CHECK([whether truncl works according to ISO C 99 with IEC 60559],
          [gl_cv_func_truncl_ieee],
          [
            saved_LIBS="$LIBS"
            LIBS="$LIBS $TRUNCL_LIBM"
            AC_RUN_IFELSE(
              [AC_LANG_SOURCE([[
#ifndef __NO_MATH_INLINES
# define __NO_MATH_INLINES 1 /* for glibc */
#endif
#include <math.h>
]gl_LONG_DOUBLE_MINUS_ZERO_CODE[
]gl_LONG_DOUBLE_SIGNBIT_CODE[
static long double dummy (long double f) { return 0; }
int main (int argc, char *argv[])
{
  long double (* volatile my_truncl) (long double) = argc ? truncl : dummy;
  /* Test whether truncl (-0.3L) is -0.0L.  */
  if (signbitl (minus_zerol) && !signbitl (my_truncl (-0.3L)))
    return 1;
  return 0;
}
              ]])],
              [gl_cv_func_truncl_ieee=yes],
              [gl_cv_func_truncl_ieee=no],
              [case "$host_os" in
                                     # Guess yes on glibc systems.
                 *-gnu* | gnu*)      gl_cv_func_truncl_ieee="guessing yes" ;;
                                     # Guess yes on musl systems.
                 *-musl* | midipix*) gl_cv_func_truncl_ieee="guessing yes" ;;
                                     # Guess yes on native Windows.
                 mingw* | windows*)  gl_cv_func_truncl_ieee="guessing yes" ;;
                                     # If we don't know, obey --enable-cross-guesses.
                 *)                  gl_cv_func_truncl_ieee="$gl_cross_guess_normal" ;;
               esac
              ])
            LIBS="$saved_LIBS"
          ])
        case "$gl_cv_func_truncl_ieee" in
          *yes) ;;
          *) REPLACE_TRUNCL=1 ;;
        esac
      fi
    ])
  else
    HAVE_DECL_TRUNCL=0
  fi
  if test $HAVE_DECL_TRUNCL = 0 || test $REPLACE_TRUNCL = 1; then
    dnl Find libraries needed to link lib/truncl.c.
    if test $HAVE_SAME_LONG_DOUBLE_AS_DOUBLE = 1; then
      AC_REQUIRE([gl_FUNC_TRUNC])
      TRUNCL_LIBM="$TRUNC_LIBM"
    else
      TRUNCL_LIBM=
    fi
  fi
  AC_SUBST([TRUNCL_LIBM])
])
