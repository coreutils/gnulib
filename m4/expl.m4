# expl.m4 serial 5
dnl Copyright (C) 2010-2011 Free Software Foundation, Inc.
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
             long double x;]],
           [[return expl (x) > 1.5;]])],
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
               long double x;]],
             [[return expl (x) > 1.5;]])],
          [gl_cv_func_expl_in_libm=yes],
          [gl_cv_func_expl_in_libm=no])
        LIBS="$save_LIBS"
      ])
    if test $gl_cv_func_expl_in_libm = yes; then
      EXPL_LIBM=-lm
    fi
  fi
  if test $gl_cv_func_expl_no_libm = yes \
     || test $gl_cv_func_expl_in_libm = yes; then
    dnl Also check whether it's declared.
    dnl MacOS X 10.3 has expl() in libc but doesn't declare it in <math.h>.
    AC_CHECK_DECL([expl], , [HAVE_DECL_EXPL=0], [[#include <math.h>]])
  else
    HAVE_DECL_EXPL=0
    HAVE_EXPL=0
    dnl Find libraries needed to link lib/expl.c.
    if test $HAVE_SAME_LONG_DOUBLE_AS_DOUBLE = 1; then
      AC_REQUIRE([gl_FUNC_EXP])
      EXPL_LIBM="$EXP_LIBM"
    else
      AC_REQUIRE([gl_FUNC_FLOORL])
      EXPL_LIBM="$FLOORL_LIBM"
    fi
  fi
  AC_SUBST([EXPL_LIBM])
])
