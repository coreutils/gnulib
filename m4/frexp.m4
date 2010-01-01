# frexp.m4 serial 7
dnl Copyright (C) 2007, 2008, 2009, 2010 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_FREXP],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  FREXP_LIBM=
  AC_CACHE_CHECK([whether frexp() can be used without linking with libm],
    [gl_cv_func_frexp_no_libm],
    [
      AC_TRY_LINK([#include <math.h>
                   double x;],
                  [int e; return frexp (x, &e) > 0;],
        [gl_cv_func_frexp_no_libm=yes],
        [gl_cv_func_frexp_no_libm=no])
    ])
  if test $gl_cv_func_frexp_no_libm = no; then
    AC_CACHE_CHECK([whether frexp() can be used with libm],
      [gl_cv_func_frexp_in_libm],
      [
        save_LIBS="$LIBS"
        LIBS="$LIBS -lm"
        AC_TRY_LINK([#include <math.h>
                     double x;],
                    [int e; return frexp (x, &e) > 0;],
          [gl_cv_func_frexp_in_libm=yes],
          [gl_cv_func_frexp_in_libm=no])
        LIBS="$save_LIBS"
      ])
    if test $gl_cv_func_frexp_in_libm = yes; then
      FREXP_LIBM=-lm
    fi
  fi
  if test $gl_cv_func_frexp_no_libm = yes \
     || test $gl_cv_func_frexp_in_libm = yes; then
    save_LIBS="$LIBS"
    LIBS="$LIBS $FREXP_LIBM"
    gl_FUNC_FREXP_WORKS
    LIBS="$save_LIBS"
    case "$gl_cv_func_frexp_works" in
      *yes) gl_func_frexp=yes ;;
      *)    gl_func_frexp=no; REPLACE_FREXP=1; FREXP_LIBM= ;;
    esac
  else
    gl_func_frexp=no
  fi
  if test $gl_func_frexp = yes; then
    AC_DEFINE([HAVE_FREXP], [1],
      [Define if the frexp() function is available and works.])
  else
    AC_LIBOBJ([frexp])
  fi
  AC_SUBST([FREXP_LIBM])
])

AC_DEFUN([gl_FUNC_FREXP_NO_LIBM],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  AC_CACHE_CHECK([whether frexp() can be used without linking with libm],
    [gl_cv_func_frexp_no_libm],
    [
      AC_TRY_LINK([#include <math.h>
                   double x;],
                  [int e; return frexp (x, &e) > 0;],
        [gl_cv_func_frexp_no_libm=yes],
        [gl_cv_func_frexp_no_libm=no])
    ])
  if test $gl_cv_func_frexp_no_libm = yes; then
    gl_FUNC_FREXP_WORKS
    case "$gl_cv_func_frexp_works" in
      *yes) gl_func_frexp_no_libm=yes ;;
      *)    gl_func_frexp_no_libm=no; REPLACE_FREXP=1 ;;
    esac
  else
    gl_func_frexp_no_libm=no
    dnl Set REPLACE_FREXP here because the system may have frexp in libm.
    REPLACE_FREXP=1
  fi
  if test $gl_func_frexp_no_libm = yes; then
    AC_DEFINE([HAVE_FREXP_IN_LIBC], [1],
      [Define if the frexp() function is available in libc.])
  else
    AC_LIBOBJ([frexp])
  fi
])

dnl Test whether frexp() works also on denormalized numbers (this fails e.g. on
dnl NetBSD 3.0), on infinite numbers (this fails e.g. on IRIX 6.5 and mingw),
dnl and on negative zero (this fails e.g. on NetBSD 4.99).
AC_DEFUN([gl_FUNC_FREXP_WORKS],
[
  AC_REQUIRE([AC_PROG_CC])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  AC_CACHE_CHECK([whether frexp works], [gl_cv_func_frexp_works],
    [
      AC_TRY_RUN([
#include <float.h>
#include <math.h>
#include <string.h>
int main()
{
  int i;
  volatile double x;
/* HP cc on HP-UX 10.20 has a bug with the constant expression -0.0.
   So we use -zero instead.  */
  double zero = 0.0;
  /* Test on denormalized numbers.  */
  for (i = 1, x = 1.0; i >= DBL_MIN_EXP; i--, x *= 0.5)
    ;
  if (x > 0.0)
    {
      int exp;
      double y = frexp (x, &exp);
      /* On machines with IEEE754 arithmetic: x = 1.11254e-308, exp = -1022.
         On NetBSD: y = 0.75. Correct: y = 0.5.  */
      if (y != 0.5)
        return 1;
    }
  /* Test on infinite numbers.  */
  x = 1.0 / 0.0;
  {
    int exp;
    double y = frexp (x, &exp);
    if (y != x)
      return 1;
  }
  /* Test on negative zero.  */
  x = -zero;
  {
    int exp;
    double y = frexp (x, &exp);
    if (memcmp (&y, &x, sizeof x))
      return 1;
  }
  return 0;
}], [gl_cv_func_frexp_works=yes], [gl_cv_func_frexp_works=no],
      [case "$host_os" in
         netbsd* | irix* | mingw*) gl_cv_func_frexp_works="guessing no";;
         *)                        gl_cv_func_frexp_works="guessing yes";;
       esac
      ])
    ])
])
