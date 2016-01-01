# ilogb.m4 serial 1
dnl Copyright (C) 2010-2016 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_ILOGB],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])

  dnl Determine ILOGB_LIBM.
  gl_MATHFUNC([ilogb], [int], [(double)])
  if test $gl_cv_func_ilogb_no_libm = yes \
     || test $gl_cv_func_ilogb_in_libm = yes; then
    save_LIBS="$LIBS"
    LIBS="$LIBS $ILOGB_LIBM"
    gl_FUNC_ILOGB_WORKS
    LIBS="$save_LIBS"
    case "$gl_cv_func_ilogb_works" in
      *yes) ;;
      *) REPLACE_ILOGB=1 ;;
    esac
  else
    HAVE_ILOGB=0
  fi
  if test $HAVE_ILOGB = 0 || test $REPLACE_ILOGB = 1; then
    dnl Find libraries needed to link lib/ilogb.c.
    AC_REQUIRE([gl_FUNC_FREXP])
    AC_REQUIRE([gl_FUNC_ISNAND])
    ILOGB_LIBM=
    dnl Append $FREXP_LIBM to ILOGB_LIBM, avoiding gratuitous duplicates.
    case " $ILOGB_LIBM " in
      *" $FREXP_LIBM "*) ;;
      *) ILOGB_LIBM="$ILOGB_LIBM $FREXP_LIBM" ;;
    esac
    dnl Append $ISNAND_LIBM to ILOGB_LIBM, avoiding gratuitous duplicates.
    case " $ILOGB_LIBM " in
      *" $ISNAND_LIBM "*) ;;
      *) ILOGB_LIBM="$ILOGB_LIBM $ISNAND_LIBM" ;;
    esac
  fi
  AC_SUBST([ILOGB_LIBM])
])

dnl Test whether ilogb() works.
dnl On OpenBSD 4.9, AIX 5.1, ilogb(0.0) is wrong.
dnl On NetBSD 5.1, OpenBSD 4.9, ilogb(Infinity) is wrong.
dnl On OpenBSD 4.9, ilogb(NaN) is wrong.
AC_DEFUN([gl_FUNC_ILOGB_WORKS],
[
  AC_REQUIRE([AC_PROG_CC])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  AC_CACHE_CHECK([whether ilogb works], [gl_cv_func_ilogb_works],
    [
      AC_RUN_IFELSE(
        [AC_LANG_SOURCE([[
#include <limits.h>
#include <math.h>
/* Provide FP_ILOGB0, FP_ILOGBNAN, like in math.in.h.  */
#if !(defined FP_ILOGB0 && defined FP_ILOGBNAN)
# if defined __NetBSD__ || defined __sgi
  /* NetBSD, IRIX 6.5: match what ilogb() does */
#  define FP_ILOGB0   INT_MIN
#  define FP_ILOGBNAN INT_MIN
# elif defined _AIX
  /* AIX 5.1: match what ilogb() does in AIX >= 5.2 */
#  define FP_ILOGB0   INT_MIN
#  define FP_ILOGBNAN INT_MAX
# elif defined __sun
  /* Solaris 9: match what ilogb() does */
#  define FP_ILOGB0   (- INT_MAX)
#  define FP_ILOGBNAN INT_MAX
# endif
#endif
volatile double x;
static double zero;
static int dummy (double x) { return 0; }
int main (int argc, char *argv[])
{
  int (*my_ilogb) (double) = argc ? ilogb : dummy;
  int result = 0;
  /* This test fails on OpenBSD 4.9, AIX 5.1.  */
  {
    x = 0.0;
    if (my_ilogb (x) != FP_ILOGB0)
      result |= 1;
  }
  /* This test fails on NetBSD 5.1, OpenBSD 4.9.  */
  {
    x = 1.0 / zero;
    if (my_ilogb (x) != INT_MAX)
      result |= 2;
  }
  /* This test fails on OpenBSD 4.9.  */
  {
    x = zero / zero;
    if (my_ilogb (x) != FP_ILOGBNAN)
      result |= 4;
  }
  return result;
}
]])],
        [gl_cv_func_ilogb_works=yes],
        [gl_cv_func_ilogb_works=no],
        [case "$host_os" in
           aix* | openbsd* | netbsd* | solaris*)
              gl_cv_func_ilogb_works="guessing no";;
           *) gl_cv_func_ilogb_works="guessing yes";;
         esac
        ])
    ])
])
