# ldexpl.m4 serial 2
dnl Copyright (C) 2007 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_LDEXPL],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  LDEXPL_LIBM=
  AC_CACHE_CHECK([whether ldexpl() can be used without linking with libm],
    [gl_cv_func_ldexpl_no_libm],
    [
      AC_TRY_LINK([#include <math.h>
                   long double x;],
                  [return ldexpl (x, -1) > 0;],
        [gl_cv_func_ldexpl_no_libm=yes],
        [gl_cv_func_ldexpl_no_libm=no])
    ])
  if test $gl_cv_func_ldexpl_no_libm = no; then
    AC_CACHE_CHECK([whether ldexpl() can be used with libm],
      [gl_cv_func_ldexpl_in_libm],
      [
        save_LIBS="$LIBS"
        LIBS="$LIBS -lm"
        AC_TRY_LINK([#include <math.h>
                     long double x;],
                    [return ldexpl (x, -1) > 0;],
          [gl_cv_func_ldexpl_in_libm=yes],
          [gl_cv_func_ldexpl_in_libm=no])
        LIBS="$save_LIBS"
      ])
    if test $gl_cv_func_ldexpl_in_libm = yes; then
      LDEXPL_LIBM=-lm
    fi
  fi
  if test $gl_cv_func_ldexpl_no_libm = yes \
     || test $gl_cv_func_ldexpl_in_libm = yes; then
    save_LIBS="$LIBS"
    LIBS="$LIBS $LDEXPL_LIBM"
    gl_FUNC_LDEXPL_WORKS
    LIBS="$save_LIBS"
    case "$gl_cv_func_ldexpl_works" in
      *yes) gl_func_ldexpl=yes ;;
      *)    gl_func_ldexpl=no; REPLACE_LDEXPL=1; LDEXPL_LIBM= ;;
    esac
  else
    gl_func_ldexpl=no
  fi
  if test $gl_func_ldexpl = yes; then
    AC_DEFINE([HAVE_LDEXPL], 1,
      [Define if the ldexpl() function is available.])
    dnl Also check whether it's declared.
    dnl MacOS X 10.3 has ldexpl() in libc but doesn't declare it in <math.h>.
    AC_CHECK_DECL([ldexpl], , [HAVE_DECL_LDEXPL=0], [#include <math.h>])
  else
    HAVE_DECL_LDEXPL=0
    AC_LIBOBJ([ldexpl])
  fi
  AC_SUBST([LDEXPL_LIBM])
])

dnl Test whether ldexpl() works on finite numbers (this fails on AIX 5.1).
AC_DEFUN([gl_FUNC_LDEXPL_WORKS],
[
  AC_REQUIRE([AC_PROG_CC])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  AC_CACHE_CHECK([whether ldexpl works], [gl_cv_func_ldexpl_works],
    [
      AC_TRY_RUN([
#include <math.h>
extern long double ldexpl (long double, int);
int main()
{
  volatile long double x = 1.0;
  volatile long double y = ldexpl (x, -1);
  return (y != 0.5L);
}], [gl_cv_func_ldexpl_works=yes], [gl_cv_func_ldexpl_works=no],
      [case "$host_os" in
         aix*) gl_cv_func_ldexpl_works="guessing no";;
         *)    gl_cv_func_ldexpl_works="guessing yes";;
       esac
      ])
    ])
])
