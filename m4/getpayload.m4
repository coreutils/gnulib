# getpayload.m4
# serial 2
dnl Copyright 2024-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN([gl_FUNC_GETPAYLOADF],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles

  gl_MATHFUNC([getpayloadf], [float], [(float const *)])
  if test $gl_cv_func_getpayloadf_no_libm != yes \
     && test $gl_cv_func_getpayloadf_in_libm != yes; then
    HAVE_GETPAYLOADF=0
  else
    dnl glibc versions < 2.32 return a wrong value,
    dnl see <https://sourceware.org/PR26073>.
    AC_CACHE_CHECK([whether getpayloadf works],
      [gl_cv_func_getpayloadf_works],
      [
       saved_LIBS="$LIBS"
       if test $gl_cv_func_getpayloadf_no_libm != yes; then
         LIBS="$LIBS -lm"
       fi
       AC_RUN_IFELSE(
         [AC_LANG_PROGRAM(
            [[#include <math.h>
            ]],
            [[float x = 2.718281828459045f;
              return getpayloadf (&x) != -1.0f;
            ]])
         ],
         [gl_cv_func_getpayloadf_works=yes],
         [gl_cv_func_getpayloadf_works=no],
         [case "$host_os" in
            # Guess no on glibc versions < 2.32.
            *-gnu* | gnu*)
              AC_EGREP_CPP([Unlucky],
                [
#include <features.h>
#ifdef __GNU_LIBRARY__
 #if (__GLIBC__ == 2 && __GLIBC_MINOR__ < 32)
  Unlucky GNU user
 #endif
#endif
                ],
                [gl_cv_func_getpayloadf_works="guessing no"],
                [gl_cv_func_getpayloadf_works="guessing yes"])
              ;;
            # Guess yes otherwise.
            *) gl_cv_func_getpayloadf_works="guessing yes" ;;
          esac
         ])
       LIBS="$saved_LIBS"
      ])
    case "$gl_cv_func_getpayloadf_works" in
      *yes) ;;
      *) REPLACE_GETPAYLOADF=1 ;;
    esac
  fi
  if test $HAVE_GETPAYLOADF = 0 || test $REPLACE_GETPAYLOADF = 1; then
    GETPAYLOADF_LIBM='$(ISNANF_LIBM)'
  fi
  AC_SUBST([GETPAYLOADF_LIBM])
])

AC_DEFUN_ONCE([gl_FUNC_GETPAYLOAD],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles

  gl_MATHFUNC([getpayload], [double], [(double const *)])
  if test $gl_cv_func_getpayload_no_libm != yes \
     && test $gl_cv_func_getpayload_in_libm != yes; then
    HAVE_GETPAYLOAD=0
  else
    dnl glibc versions < 2.32 return a wrong value,
    dnl see <https://sourceware.org/PR26073>.
    AC_CACHE_CHECK([whether getpayload works],
      [gl_cv_func_getpayload_works],
      [
       saved_LIBS="$LIBS"
       if test $gl_cv_func_getpayload_no_libm != yes; then
         LIBS="$LIBS -lm"
       fi
       AC_RUN_IFELSE(
         [AC_LANG_PROGRAM(
            [[#include <math.h>
            ]],
            [[double x = 2.718281828459045;
              return getpayload (&x) != -1.0;
            ]])
         ],
         [gl_cv_func_getpayload_works=yes],
         [gl_cv_func_getpayload_works=no],
         [case "$host_os" in
            # Guess no on glibc versions < 2.32.
            *-gnu* | gnu*)
              AC_EGREP_CPP([Unlucky],
                [
#include <features.h>
#ifdef __GNU_LIBRARY__
 #if (__GLIBC__ == 2 && __GLIBC_MINOR__ < 32)
  Unlucky GNU user
 #endif
#endif
                ],
                [gl_cv_func_getpayload_works="guessing no"],
                [gl_cv_func_getpayload_works="guessing yes"])
              ;;
            # Guess yes otherwise.
            *) gl_cv_func_getpayload_works="guessing yes" ;;
          esac
         ])
       LIBS="$saved_LIBS"
      ])
    case "$gl_cv_func_getpayload_works" in
      *yes) ;;
      *) REPLACE_GETPAYLOAD=1 ;;
    esac
  fi
  if test $HAVE_GETPAYLOAD = 0 || test $REPLACE_GETPAYLOAD = 1; then
    GETPAYLOAD_LIBM='$(ISNAND_LIBM)'
  fi
  AC_SUBST([GETPAYLOAD_LIBM])
])

AC_DEFUN([gl_FUNC_GETPAYLOADL],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  AC_REQUIRE([gl_LONG_DOUBLE_VS_DOUBLE])

  gl_MATHFUNC([getpayloadl], [long double], [(long double const *)])
  if test $gl_cv_func_getpayloadl_no_libm != yes \
     && test $gl_cv_func_getpayloadl_in_libm != yes; then
    HAVE_GETPAYLOADL=0
  else
    dnl glibc versions < 2.32 return a wrong value,
    dnl see <https://sourceware.org/PR26073>.
    AC_CACHE_CHECK([whether getpayloadl works],
      [gl_cv_func_getpayloadl_works],
      [
       saved_LIBS="$LIBS"
       if test $gl_cv_func_getpayloadl_no_libm != yes; then
         LIBS="$LIBS -lm"
       fi
       AC_RUN_IFELSE(
         [AC_LANG_PROGRAM(
            [[#include <math.h>
            ]],
            [[long double x = 2.718281828459045L;
              return getpayloadl (&x) != -1.0L;
            ]])
         ],
         [gl_cv_func_getpayloadl_works=yes],
         [gl_cv_func_getpayloadl_works=no],
         [case "$host_os" in
            # Guess no on glibc versions < 2.32.
            *-gnu* | gnu*)
              AC_EGREP_CPP([Unlucky],
                [
#include <features.h>
#ifdef __GNU_LIBRARY__
 #if (__GLIBC__ == 2 && __GLIBC_MINOR__ < 32)
  Unlucky GNU user
 #endif
#endif
                ],
                [gl_cv_func_getpayloadl_works="guessing no"],
                [gl_cv_func_getpayloadl_works="guessing yes"])
              ;;
            # Guess yes otherwise.
            *) gl_cv_func_getpayloadl_works="guessing yes" ;;
          esac
         ])
       LIBS="$saved_LIBS"
      ])
    case "$gl_cv_func_getpayloadl_works" in
      *yes) ;;
      *) REPLACE_GETPAYLOADL=1 ;;
    esac
  fi
  if test $HAVE_GETPAYLOADL = 0 || test $REPLACE_GETPAYLOADL = 1; then
    dnl Find libraries needed to link lib/getpayloadl.c.
    if test $HAVE_SAME_LONG_DOUBLE_AS_DOUBLE = 1; then
      AC_REQUIRE([gl_FUNC_GETPAYLOAD])
      GETPAYLOADL_LIBM="$GETPAYLOAD_LIBM"
    else
      GETPAYLOADL_LIBM='$(ISNANL_LIBM)'
    fi
    dnl Prerequisite of lib/getpayloadl.c.
    gl_LONG_DOUBLE_EXPONENT_LOCATION
  fi
  AC_SUBST([GETPAYLOADL_LIBM])
])
