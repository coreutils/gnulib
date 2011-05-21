# logl.m4 serial 4
dnl Copyright (C) 2010-2011 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_LOGL],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
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
             long double x;]],
           [[return logl (x) > 1;]])],
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
               long double x;]],
             [[return logl (x) > 1;]])],
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
    dnl MacOS X 10.3 has logl() in libc but doesn't declare it in <math.h>.
    AC_CHECK_DECL([logl], , [HAVE_DECL_LOGL=0], [#include <math.h>])
  else
    HAVE_DECL_LOGL=0
    HAVE_LOGL=0
    dnl Find libraries needed to link lib/logl.c.
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
  AC_SUBST([LOGL_LIBM])
])
