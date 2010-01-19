# mathfunc.m4 serial 1
dnl Copyright (C) 2010 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

# gl_MATHFUNC([sqrt])
# tests whether the sqrt function is available in libc or libm. It sets
# SQRT_LIBM to empty or "-lm" accordingly.

AC_DEFUN([gl_MATHFUNC],
[
  m4_pushdef([func], [$1])
  m4_pushdef([FUNC], [translit([$1],[abcdefghijklmnopqrstuvwxyz],
                                    [ABCDEFGHIJKLMNOPQRSTUVWXYZ])])
  FUNC[]_LIBM=
  AC_CACHE_CHECK([whether func() can be used without linking with libm],
    [gl_cv_func_]func[_no_libm],
    [
      AC_LINK_IFELSE(
        [AC_LANG_PROGRAM([[#ifndef __NO_MATH_INLINES
                           # define __NO_MATH_INLINES 1 /* for glibc */
                           #endif
                           #include <math.h>
                           double x;]],
                         [[return ]func[ (x) > 2 || ]func[ (x) < 0.4;]])],
        [gl_cv_func_]func[_no_libm=yes],
        [gl_cv_func_]func[_no_libm=no])
    ])
  if test $gl_cv_func_[]func[]_no_libm = no; then
    AC_CACHE_CHECK([whether func() can be used with libm],
      [gl_cv_func_]func[_in_libm],
      [
        save_LIBS="$LIBS"
        LIBS="$LIBS -lm"
        AC_LINK_IFELSE(
          [AC_LANG_PROGRAM([[#ifndef __NO_MATH_INLINES
                             # define __NO_MATH_INLINES 1 /* for glibc */
                             #endif
                             #include <math.h>
                             double x;]],
                           [[return ]func[ (x) > 2 || ]func[ (x) < 0.4;]])],
          [gl_cv_func_]func[_in_libm=yes],
          [gl_cv_func_]func[_in_libm=no])
        LIBS="$save_LIBS"
      ])
    if test $gl_cv_func_[]func[]_in_libm = yes; then
      FUNC[]_LIBM=-lm
    fi
  fi
  m4_popdef([FUNC])
  m4_popdef([func])
])
