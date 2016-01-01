# ilogbf.m4 serial 1
dnl Copyright (C) 2010-2016 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_ILOGBF],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])

  dnl Persuade glibc <math.h> to declare ilogbf().
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])

  dnl Determine ILOGBF_LIBM.
  gl_MATHFUNC([ilogbf], [int], [(float)])
  if test $gl_cv_func_ilogbf_no_libm = yes \
     || test $gl_cv_func_ilogbf_in_libm = yes; then
    save_LIBS="$LIBS"
    LIBS="$LIBS $ILOGBF_LIBM"
    gl_FUNC_ILOGBF_WORKS
    LIBS="$save_LIBS"
    case "$gl_cv_func_ilogbf_works" in
      *yes) ;;
      *) REPLACE_ILOGBF=1 ;;
    esac
  else
    HAVE_ILOGBF=0
  fi
  if test $HAVE_ILOGBF = 0 || test $REPLACE_ILOGBF = 1; then
    dnl Find libraries needed to link lib/ilogbf.c.
    AC_REQUIRE([gl_FUNC_FREXPF])
    AC_REQUIRE([gl_FUNC_ISNANF])
    ILOGBF_LIBM=
    dnl Append $FREXPF_LIBM to ILOGBF_LIBM, avoiding gratuitous duplicates.
    case " $ILOGBF_LIBM " in
      *" $FREXPF_LIBM "*) ;;
      *) ILOGBF_LIBM="$ILOGBF_LIBM $FREXPF_LIBM" ;;
    esac
    dnl Append $ISNANF_LIBM to ILOGBF_LIBM, avoiding gratuitous duplicates.
    case " $ILOGBF_LIBM " in
      *" $ISNANF_LIBM "*) ;;
      *) ILOGBF_LIBM="$ILOGBF_LIBM $ISNANF_LIBM" ;;
    esac
  fi
  AC_SUBST([ILOGBF_LIBM])
])

dnl Test whether ilogbf() works.
dnl On OpenBSD 4.9, ilogbf(0.0) is wrong.
dnl On NetBSD 5.1, OpenBSD 4.9, ilogbf(Infinity) is wrong.
AC_DEFUN([gl_FUNC_ILOGBF_WORKS],
[
  AC_REQUIRE([AC_PROG_CC])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  AC_CACHE_CHECK([whether ilogbf works], [gl_cv_func_ilogbf_works],
    [
      AC_RUN_IFELSE(
        [AC_LANG_SOURCE([[
#include <limits.h>
#include <math.h>
/* Provide FP_ILOGB0, FP_ILOGBNAN, like in math.in.h.  */
#if !(defined FP_ILOGB0 && defined FP_ILOGBNAN)
# if defined __NetBSD__ || defined __sgi
  /* NetBSD, IRIX 6.5: match what ilogbf() does */
#  define FP_ILOGB0   INT_MIN
#  define FP_ILOGBNAN INT_MIN
# elif defined _AIX
  /* AIX 5.1: match what ilogbf() does in AIX >= 5.2 */
#  define FP_ILOGB0   INT_MIN
#  define FP_ILOGBNAN INT_MAX
# elif defined __sun
  /* Solaris 9: match what ilogbf() does */
#  define FP_ILOGB0   (- INT_MAX)
#  define FP_ILOGBNAN INT_MAX
# endif
#endif
volatile float x;
static float zero;
static int dummy (float x) { return 0; }
int main (int argc, char *argv[])
{
  int (*my_ilogbf) (float) = argc ? ilogbf : dummy;
  int result = 0;
  /* This test fails on OpenBSD 4.9.  */
  {
    x = 0.0f;
    if (my_ilogbf (x) != FP_ILOGB0)
      result |= 1;
  }
  /* This test fails on NetBSD 5.1, OpenBSD 4.9.  */
  {
    x = 1.0f / zero;
    if (my_ilogbf (x) != INT_MAX)
      result |= 2;
  }
  return result;
}
]])],
        [gl_cv_func_ilogbf_works=yes],
        [gl_cv_func_ilogbf_works=no],
        [case "$host_os" in
           openbsd* | netbsd*) gl_cv_func_ilogbf_works="guessing no";;
           *)                  gl_cv_func_ilogbf_works="guessing yes";;
         esac
        ])
    ])
])
