# sqrtl.m4 serial 4
dnl Copyright (C) 2010-2011 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_SQRTL],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  dnl Persuade glibc <math.h> to declare sqrtl().
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])

  SQRTL_LIBM=
  AC_CACHE_CHECK([whether sqrtl() can be used without linking with libm],
    [gl_cv_func_sqrtl_no_libm],
    [
      AC_LINK_IFELSE(
        [AC_LANG_PROGRAM(
           [[#ifndef __NO_MATH_INLINES
             # define __NO_MATH_INLINES 1 /* for glibc */
             #endif
             #include <math.h>
             long double x;]],
           [[return sqrtl (x) > 0.4;]])],
        [gl_cv_func_sqrtl_no_libm=yes],
        [gl_cv_func_sqrtl_no_libm=no])
    ])
  if test $gl_cv_func_sqrtl_no_libm = no; then
    AC_CACHE_CHECK([whether sqrtl() can be used with libm],
      [gl_cv_func_sqrtl_in_libm],
      [
        save_LIBS="$LIBS"
        LIBS="$LIBS -lm"
        AC_LINK_IFELSE(
          [AC_LANG_PROGRAM(
             [[#ifndef __NO_MATH_INLINES
               # define __NO_MATH_INLINES 1 /* for glibc */
               #endif
               #include <math.h>
               long double x;]],
             [[return sqrtl (x) > 0.4;]])],
          [gl_cv_func_sqrtl_in_libm=yes],
          [gl_cv_func_sqrtl_in_libm=no])
        LIBS="$save_LIBS"
      ])
    if test $gl_cv_func_sqrtl_in_libm = yes; then
      SQRTL_LIBM=-lm
    fi
  fi
  if test $gl_cv_func_sqrtl_no_libm = yes \
     || test $gl_cv_func_sqrtl_in_libm = yes; then
    dnl Also check whether it's declared.
    dnl MacOS X 10.3 has sqrtl() in libc but doesn't declare it in <math.h>.
    AC_CHECK_DECL([sqrtl], , [HAVE_DECL_SQRTL=0], [#include <math.h>])
  else
    HAVE_DECL_SQRTL=0
    HAVE_SQRTL=0
    dnl Find libraries needed to link lib/sqrtl.c.
    AC_REQUIRE([gl_FUNC_ISNANL])
    AC_REQUIRE([gl_FUNC_FREXPL])
    AC_REQUIRE([gl_FUNC_LDEXPL])
    AC_REQUIRE([gl_FUNC_SQRT])
    dnl Append $ISNANL_LIBM to SQRTL_LIBM, avoiding gratuitous duplicates.
    case " $SQRTL_LIBM " in
      *" $ISNANL_LIBM "*) ;;
      *) SQRTL_LIBM="$SQRTL_LIBM $ISNANL_LIBM" ;;
    esac
    dnl Append $FREXPL_LIBM to SQRTL_LIBM, avoiding gratuitous duplicates.
    case " $SQRTL_LIBM " in
      *" $FREXPL_LIBM "*) ;;
      *) SQRTL_LIBM="$SQRTL_LIBM $FREXPL_LIBM" ;;
    esac
    dnl Append $LDEXPL_LIBM to SQRTL_LIBM, avoiding gratuitous duplicates.
    case " $SQRTL_LIBM " in
      *" $LDEXPL_LIBM "*) ;;
      *) SQRTL_LIBM="$SQRTL_LIBM $LDEXPL_LIBM" ;;
    esac
    dnl Append $SQRT_LIBM to SQRTL_LIBM, avoiding gratuitous duplicates.
    case " $SQRTL_LIBM " in
      *" $SQRT_LIBM "*) ;;
      *) SQRTL_LIBM="$SQRTL_LIBM $SQRT_LIBM" ;;
    esac
  fi
  AC_SUBST([SQRTL_LIBM])
])
