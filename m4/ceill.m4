# ceill.m4 serial 14
dnl Copyright (C) 2007, 2009-2016 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_CEILL],
[
  m4_divert_text([DEFAULTS], [gl_ceill_required=plain])
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  AC_REQUIRE([gl_LONG_DOUBLE_VS_DOUBLE])

  dnl Persuade glibc <math.h> to declare ceill().
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])

  dnl Test whether ceill() is declared.
  AC_CHECK_DECLS([ceill], , , [[#include <math.h>]])
  if test "$ac_cv_have_decl_ceill" = yes; then
    dnl Test whether ceill() can be used without libm.
    gl_FUNC_CEILL_LIBS
    if test "$CEILL_LIBM" = "?"; then
      dnl Sun C 5.0 on Solaris declares ceill() and has it in the system-wide
      dnl libm.so, but not in the libm.so that the compiler uses.
      REPLACE_CEILL=1
    fi
    m4_ifdef([gl_FUNC_CEILL_IEEE], [
      if test $gl_ceill_required = ieee && test $REPLACE_CEILL = 0; then
        AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
        AC_CACHE_CHECK([whether ceill works according to ISO C 99 with IEC 60559],
          [gl_cv_func_ceill_ieee],
          [
            save_LIBS="$LIBS"
            LIBS="$LIBS $CEILL_LIBM"
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
  long double (*my_ceill) (long double) = argc ? ceill : dummy;
  /* Test whether ceill (-0.3L) is -0.0L.  */
  if (signbitl (minus_zerol) && !signbitl (my_ceill (-0.3L)))
    return 1;
  return 0;
}
              ]])],
              [gl_cv_func_ceill_ieee=yes],
              [gl_cv_func_ceill_ieee=no],
              [case "$host_os" in
                         # Guess yes on glibc systems.
                 *-gnu*) gl_cv_func_ceill_ieee="guessing yes" ;;
                         # If we don't know, assume the worst.
                 *)      gl_cv_func_ceill_ieee="guessing no" ;;
               esac
              ])
            LIBS="$save_LIBS"
          ])
        case "$gl_cv_func_ceill_ieee" in
          *yes) ;;
          *) REPLACE_CEILL=1 ;;
        esac
      fi
    ])
  else
    HAVE_DECL_CEILL=0
  fi
  dnl On OpenBSD5.6 the system's native ceill() is buggy:
  dnl it returns '0' for small values. Test for this anomaly.
  if test $REPLACE_CEILL = 0 ; then
    AC_CACHE_CHECK([whether ceill() breaks with small values],
        [gl_cv_func_ceill_buggy],
        [
          save_LIBS="$LIBS"
          LIBS="$CEILL_LIBM"
          AC_RUN_IFELSE(
           [AC_LANG_PROGRAM(
             [[#include <math.h>
long double d = 0.3L;]],
             [[return (!(ceill (d) == 1)); ]])],
             [gl_cv_func_ceill_buggy=no], [gl_cv_func_ceill_buggy=yes],
             [case $host_os in
                openbsd*) gl_cv_func_ceill_buggy="guessing yes";;
                *) gl_cv_func_ceill_buggy="guessing no";;
              esac])
          LIBS="$save_LIBS"
        ])
    case "$gl_cv_func_ceill_buggy" in
      *yes)
        REPLACE_CEILL=1 ;;
    esac
  fi
  if test $HAVE_DECL_CEILL = 0 || test $REPLACE_CEILL = 1; then
    dnl Find libraries needed to link lib/ceill.c.
    if test $HAVE_SAME_LONG_DOUBLE_AS_DOUBLE = 1; then
      AC_REQUIRE([gl_FUNC_CEIL])
      CEILL_LIBM="$CEIL_LIBM"
    else
      CEILL_LIBM=
    fi
  fi
  AC_SUBST([CEILL_LIBM])
])

# Determines the libraries needed to get the ceill() function.
# Sets CEILL_LIBM.
AC_DEFUN([gl_FUNC_CEILL_LIBS],
[
  gl_CACHE_VAL_SILENT([gl_cv_func_ceill_libm], [
    gl_cv_func_ceill_libm=?
    AC_LINK_IFELSE(
      [AC_LANG_PROGRAM(
         [[#ifndef __NO_MATH_INLINES
           # define __NO_MATH_INLINES 1 /* for glibc */
           #endif
           #include <math.h>
           long double (*funcptr) (long double) = ceill;
           long double x;]],
         [[x = funcptr(x) + ceill(x);]])],
      [gl_cv_func_ceill_libm=])
    if test "$gl_cv_func_ceill_libm" = "?"; then
      save_LIBS="$LIBS"
      LIBS="$LIBS -lm"
      AC_LINK_IFELSE(
        [AC_LANG_PROGRAM(
           [[#ifndef __NO_MATH_INLINES
             # define __NO_MATH_INLINES 1 /* for glibc */
             #endif
             #include <math.h>
             long double (*funcptr) (long double) = ceill;
             long double x;]],
           [[x = funcptr(x) + ceill(x);]])],
        [gl_cv_func_ceill_libm="-lm"])
      LIBS="$save_LIBS"
    fi
  ])
  CEILL_LIBM="$gl_cv_func_ceill_libm"
])
