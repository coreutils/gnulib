# logl.m4 serial 9
dnl Copyright (C) 2010-2016 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_LOGL],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  AC_REQUIRE([gl_LONG_DOUBLE_VS_DOUBLE])

  dnl Persuade glibc <math.h> to declare logl().
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])

  LOGL_LIBM=
  AC_CACHE_CHECK([whether logl() can be used without linking with libm],
    [gl_cv_func_logl_no_libm],
    [
      AC_LINK_IFELSE(
        [AC_LANG_PROGRAM(
           [[#ifndef __NO_MATH_INLINES
             # define __NO_MATH_INLINES 1 /* for glibc */
             #endif
             #include <math.h>
             long double (*funcptr) (long double) = logl;
             long double x;]],
           [[return funcptr (x) > 1
                    || logl (x) > 1;]])],
        [gl_cv_func_logl_no_libm=yes],
        [gl_cv_func_logl_no_libm=no])
    ])
  if test $gl_cv_func_logl_no_libm = no; then
    AC_CACHE_CHECK([whether logl() can be used with libm],
      [gl_cv_func_logl_in_libm],
      [
        save_LIBS="$LIBS"
        LIBS="$LIBS -lm"
        AC_LINK_IFELSE(
          [AC_LANG_PROGRAM(
             [[#ifndef __NO_MATH_INLINES
               # define __NO_MATH_INLINES 1 /* for glibc */
               #endif
               #include <math.h>
               long double (*funcptr) (long double) = logl;
               long double x;]],
             [[return funcptr (x) > 1
                      || logl (x) > 1;]])],
          [gl_cv_func_logl_in_libm=yes],
          [gl_cv_func_logl_in_libm=no])
        LIBS="$save_LIBS"
      ])
    if test $gl_cv_func_logl_in_libm = yes; then
      LOGL_LIBM=-lm
    fi
  fi
  if test $gl_cv_func_logl_no_libm = yes \
     || test $gl_cv_func_logl_in_libm = yes; then
    dnl Also check whether it's declared.
    dnl Mac OS X 10.3 has logl() in libc but doesn't declare it in <math.h>.
    AC_CHECK_DECL([logl], , [HAVE_DECL_LOGL=0], [[#include <math.h>]])
    save_LIBS="$LIBS"
    LIBS="$LIBS $LOGL_LIBM"
    gl_FUNC_LOGL_WORKS
    LIBS="$save_LIBS"
    case "$gl_cv_func_logl_works" in
      *yes) ;;
      *) REPLACE_LOGL=1 ;;
    esac
  else
    HAVE_LOGL=0
    HAVE_DECL_LOGL=0
  fi
  if test $HAVE_LOGL = 0 || test $REPLACE_LOGL = 1; then
    dnl Find libraries needed to link lib/logl.c.
    if test $HAVE_SAME_LONG_DOUBLE_AS_DOUBLE = 1; then
      AC_REQUIRE([gl_FUNC_LOG])
      LOGL_LIBM="$LOG_LIBM"
    else
      if test $HAVE_LOGL = 0; then
        AC_REQUIRE([gl_FUNC_FREXPL])
        AC_REQUIRE([gl_FUNC_ISNANL])
        AC_REQUIRE([gl_FUNC_FLOORL])
        dnl Append $FREXPL_LIBM to LOGL_LIBM, avoiding gratuitous duplicates.
        case " $LOGL_LIBM " in
          *" $FREXPL_LIBM "*) ;;
          *) LOGL_LIBM="$LOGL_LIBM $FREXPL_LIBM" ;;
        esac
        dnl Append $ISNANL_LIBM to LOGL_LIBM, avoiding gratuitous duplicates.
        case " $LOGL_LIBM " in
          *" $ISNANL_LIBM "*) ;;
          *) LOGL_LIBM="$LOGL_LIBM $ISNANL_LIBM" ;;
        esac
        dnl Append $FLOORL_LIBM to LOGL_LIBM, avoiding gratuitous duplicates.
        case " $LOGL_LIBM " in
          *" $FLOORL_LIBM "*) ;;
          *) LOGL_LIBM="$LOGL_LIBM $FLOORL_LIBM" ;;
        esac
      fi
    fi
  fi
  AC_SUBST([LOGL_LIBM])
])

dnl Test whether logl() works.
dnl On OSF/1 5.1, logl(-0.0L) is NaN.
AC_DEFUN([gl_FUNC_LOGL_WORKS],
[
  AC_REQUIRE([AC_PROG_CC])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  AC_CACHE_CHECK([whether logl works], [gl_cv_func_logl_works],
    [
      AC_RUN_IFELSE(
        [AC_LANG_SOURCE([[
#include <math.h>
volatile long double x;
long double y;
int main ()
{
  x = -0.0L;
  y = logl (x);
  if (!(y + y == y))
    return 1;
  return 0;
}
]])],
        [gl_cv_func_logl_works=yes],
        [gl_cv_func_logl_works=no],
        [case "$host_os" in
           osf*) gl_cv_func_logl_works="guessing no";;
           *)    gl_cv_func_logl_works="guessing yes";;
         esac
        ])
    ])
])
