# remainderl.m4 serial 7
dnl Copyright (C) 2012-2016 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_REMAINDERL],
[
  m4_divert_text([DEFAULTS], [gl_remainderl_required=plain])
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  AC_REQUIRE([gl_LONG_DOUBLE_VS_DOUBLE])
  AC_REQUIRE([gl_FUNC_REMAINDER])

  dnl Persuade glibc <math.h> to declare remainderl().
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])

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
             extern
             #ifdef __cplusplus
             "C"
             #endif
             long double remainderl (long double, long double);
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
    HAVE_REMAINDERL=1
    REMAINDERL_LIBM="$REMAINDER_LIBM"
    dnl Also check whether it's declared.
    dnl IRIX 6.5 has remainderl() in libm but doesn't declare it in <math.h>.
    AC_CHECK_DECLS([remainderl], , [HAVE_DECL_REMAINDERL=0], [[#include <math.h>]])

    save_LIBS="$LIBS"
    LIBS="$LIBS $REMAINDERL_LIBM"
    gl_FUNC_REMAINDERL_WORKS
    LIBS="$save_LIBS"
    case "$gl_cv_func_remainderl_works" in
      *yes) ;;
      *) REPLACE_REMAINDERL=1 ;;
    esac

    m4_ifdef([gl_FUNC_REMAINDERL_IEEE], [
      if test $gl_remainderl_required = ieee && test $REPLACE_REMAINDERL = 0; then
        AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
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
extern
#ifdef __cplusplus
"C"
#endif
long double remainderl (long double, long double);
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
              [case "$host_os" in
                         # Guess yes on glibc systems.
                 *-gnu*) gl_cv_func_remainderl_ieee="guessing yes" ;;
                         # If we don't know, assume the worst.
                 *)      gl_cv_func_remainderl_ieee="guessing no" ;;
               esac
              ])
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
    HAVE_DECL_REMAINDERL=0
  fi
  if test $HAVE_REMAINDERL = 0 || test $REPLACE_REMAINDERL = 1; then
    dnl Find libraries needed to link lib/remainderl.c.
    if test $HAVE_SAME_LONG_DOUBLE_AS_DOUBLE = 1; then
      REMAINDERL_LIBM="$REMAINDER_LIBM"
    else
      AC_REQUIRE([gl_FUNC_FABSL])
      AC_REQUIRE([gl_FUNC_FMODL])
      AC_REQUIRE([gl_FUNC_ISNANL])
      REMAINDERL_LIBM=
      dnl Append $FABSL_LIBM to REMAINDERL_LIBM, avoiding gratuitous duplicates.
      case " $REMAINDERL_LIBM " in
        *" $FABSL_LIBM "*) ;;
        *) REMAINDERL_LIBM="$REMAINDERL_LIBM $FABSL_LIBM" ;;
      esac
      dnl Append $FMODL_LIBM to REMAINDERL_LIBM, avoiding gratuitous duplicates.
      case " $REMAINDERL_LIBM " in
        *" $FMODL_LIBM "*) ;;
        *) REMAINDERL_LIBM="$REMAINDERL_LIBM $FMODL_LIBM" ;;
      esac
      dnl Append $ISNANL_LIBM to REMAINDERL_LIBM, avoiding gratuitous duplicates.
      case " $REMAINDERL_LIBM " in
        *" $ISNANL_LIBM "*) ;;
        *) REMAINDERL_LIBM="$REMAINDERL_LIBM $ISNANL_LIBM" ;;
      esac
    fi
  fi
  AC_SUBST([REMAINDERL_LIBM])
])

dnl Test whether remainderl() works.
dnl It produces completely wrong values on OpenBSD 5.1/SPARC.
AC_DEFUN([gl_FUNC_REMAINDERL_WORKS],
[
  AC_REQUIRE([AC_PROG_CC])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  AC_CACHE_CHECK([whether remainderl works], [gl_cv_func_remainderl_works],
    [
      AC_RUN_IFELSE(
        [AC_LANG_SOURCE([[
#ifndef __NO_MATH_INLINES
# define __NO_MATH_INLINES 1 /* for glibc */
#endif
#include <math.h>
extern
#ifdef __cplusplus
"C"
#endif
long double remainderl (long double, long double);
volatile long double x;
volatile long double y;
long double z;
int main ()
{
  /* This test fails on OpenBSD 5.1/SPARC.  */
  x = 9.245907126L;
  y = 3.141592654L;
  z = remainderl (x, y);
  if (z >= 0.0L)
    return 1;
  return 0;
}
]])],
        [gl_cv_func_remainderl_works=yes],
        [gl_cv_func_remainderl_works=no],
        [case "$host_os" in
           openbsd*) gl_cv_func_remainderl_works="guessing no";;
           *)        gl_cv_func_remainderl_works="guessing yes";;
         esac
        ])
    ])
])
