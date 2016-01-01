# expl.m4 serial 8
dnl Copyright (C) 2010-2016 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_EXPL],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  AC_REQUIRE([gl_LONG_DOUBLE_VS_DOUBLE])

  dnl Persuade glibc <math.h> to declare expl().
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])

  EXPL_LIBM=
  AC_CACHE_CHECK([whether expl() can be used without linking with libm],
    [gl_cv_func_expl_no_libm],
    [
      AC_LINK_IFELSE(
        [AC_LANG_PROGRAM(
           [[#ifndef __NO_MATH_INLINES
             # define __NO_MATH_INLINES 1 /* for glibc */
             #endif
             #include <math.h>
             long double (*funcptr) (long double) = expl;
             long double x;]],
           [[return funcptr (x) > 1.5
                    || expl (x) > 1.5;]])],
        [gl_cv_func_expl_no_libm=yes],
        [gl_cv_func_expl_no_libm=no])
    ])
  if test $gl_cv_func_expl_no_libm = no; then
    AC_CACHE_CHECK([whether expl() can be used with libm],
      [gl_cv_func_expl_in_libm],
      [
        save_LIBS="$LIBS"
        LIBS="$LIBS -lm"
        AC_LINK_IFELSE(
          [AC_LANG_PROGRAM(
             [[#ifndef __NO_MATH_INLINES
               # define __NO_MATH_INLINES 1 /* for glibc */
               #endif
               #include <math.h>
               long double (*funcptr) (long double) = expl;
               long double x;]],
             [[return funcptr (x) > 1.5
                      || expl (x) > 1.5;]])],
          [gl_cv_func_expl_in_libm=yes],
          [gl_cv_func_expl_in_libm=no])
        LIBS="$save_LIBS"
      ])
    if test $gl_cv_func_expl_in_libm = yes; then
      EXPL_LIBM=-lm
    fi
  fi
  dnl On OpenBSD5.4 the system's native expl() is buggy:
  dnl it returns 'nan' for small values. Test for this anomaly.
  if test $gl_cv_func_expl_no_libm = yes \
     || test $gl_cv_func_expl_in_libm = yes; then
    AC_CACHE_CHECK([whether expl() breaks with small values],
        [gl_cv_func_expl_buggy],
        [
          save_LIBS="$LIBS"
          LIBS="$EXPL_LIBM"
          AC_RUN_IFELSE(
           [AC_LANG_PROGRAM(
             [[#include <math.h>]],
             [[return isnan(expl(-1.0))||
                      isnan(expl(-0.8))||
                      isnan(expl(-0.4)); ]])],
             [gl_cv_func_expl_buggy=no], [gl_cv_func_expl_buggy=yes],
             [case $host_os in
                openbsd*) gl_cv_func_expl_buggy="guessing yes";;
                *) gl_cv_func_expl_buggy="guessing no";;
              esac])
          LIBS="$save_LIBS"
        ])
    case "$gl_cv_func_expl_buggy" in
      *yes)
        gl_cv_func_expl_in_libm=no
        gl_cv_func_expl_no_libm=no ;;
    esac
  fi
  if test $gl_cv_func_expl_no_libm = yes \
     || test $gl_cv_func_expl_in_libm = yes; then
    dnl Also check whether it's declared.
    dnl Mac OS X 10.3 has expl() in libc but doesn't declare it in <math.h>.
    AC_CHECK_DECL([expl], , [HAVE_DECL_EXPL=0], [[#include <math.h>]])
  else
    HAVE_DECL_EXPL=0
    HAVE_EXPL=0
    dnl Find libraries needed to link lib/expl.c.
    if test $HAVE_SAME_LONG_DOUBLE_AS_DOUBLE = 1; then
      AC_REQUIRE([gl_FUNC_EXP])
      EXPL_LIBM="$EXP_LIBM"
    else
      AC_REQUIRE([gl_FUNC_ISNANL])
      AC_REQUIRE([gl_FUNC_ROUNDL])
      AC_REQUIRE([gl_FUNC_LDEXPL])
      EXPL_LIBM=
      dnl Append $ISNANL_LIBM to EXPL_LIBM, avoiding gratuitous duplicates.
      case " $EXPL_LIBM " in
        *" $ISNANL_LIBM "*) ;;
        *) EXPL_LIBM="$EXPL_LIBM $ISNANL_LIBM" ;;
      esac
      dnl Append $ROUNDL_LIBM to EXPL_LIBM, avoiding gratuitous duplicates.
      case " $EXPL_LIBM " in
        *" $ROUNDL_LIBM "*) ;;
        *) EXPL_LIBM="$EXPL_LIBM $ROUNDL_LIBM" ;;
      esac
      dnl Append $LDEXPL_LIBM to EXPL_LIBM, avoiding gratuitous duplicates.
      case " $EXPL_LIBM " in
        *" $LDEXPL_LIBM "*) ;;
        *) EXPL_LIBM="$EXPL_LIBM $LDEXPL_LIBM" ;;
      esac
    fi
  fi
  AC_SUBST([EXPL_LIBM])
])
