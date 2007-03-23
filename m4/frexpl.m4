# frexpl.m4 serial 1
dnl Copyright (C) 2007 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_FREXPL],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  FREXPL_LIBM=
  AC_CACHE_CHECK([whether frexpl() can be used without linking with libm],
    [gl_cv_func_frexpl_no_libm],
    [
      AC_TRY_LINK([#include <math.h>
                   long double x;],
                  [int e; return frexpl (x, &e) > 0;],
        [gl_cv_func_frexpl_no_libm=yes],
        [gl_cv_func_frexpl_no_libm=no])
    ])
  if test $gl_cv_func_frexpl_no_libm = no; then
    AC_CACHE_CHECK([whether frexpl() can be used with libm],
      [gl_cv_func_frexpl_in_libm],
      [
        save_LIBS="$LIBS"
        LIBS="$LIBS -lm"
        AC_TRY_LINK([#include <math.h>
                     long double x;],
                    [int e; return frexpl (x, &e) > 0;],
          [gl_cv_func_frexpl_in_libm=yes],
          [gl_cv_func_frexpl_in_libm=no])
        LIBS="$save_LIBS"
      ])
    if test $gl_cv_func_frexpl_in_libm = yes; then
      FREXPL_LIBM=-lm
    fi
  fi
  if test $gl_cv_func_frexpl_no_libm = yes \
     || test $gl_cv_func_frexpl_in_libm = yes; then
    AC_DEFINE([HAVE_FREXPL], 1,
      [Define if the frexpl() function is available.])
    dnl Also check whether it's declared.
    dnl MacOS X 10.3 has frexpl() in libc but doesn't declare it in <math.h>.
    AC_CHECK_DECL([frexpl], , [HAVE_DECL_FREXPL=0], [#include <math.h>])
  else
    AC_LIBOBJ([frexpl])
  fi
])
