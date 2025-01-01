# totalordermag.m4
# serial 1
dnl Copyright 2023-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN([gl_FUNC_TOTALORDERMAGF],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])

  dnl glibc versions < 2.31 had an incompatible declaration of this function,
  dnl see <https://sourceware.org/git/?p=glibc.git;a=commitdiff;h=42760d764649ad82f5fe45a26cbdf2c2500409f7>
  AC_CACHE_CHECK([whether totalordermagf has a non-standard declaration],
    [gl_cv_func_totalordermagf_incompatible],
    [AC_COMPILE_IFELSE(
       [AC_LANG_PROGRAM(
          [[#include <math.h>
          ]],
          [[extern
            #ifdef __cplusplus
            "C"
            #endif
            int totalordermagf (float const *, float const *);
          ]])
       ],
       [gl_cv_func_totalordermagf_incompatible=no],
       [gl_cv_func_totalordermagf_incompatible=yes])
    ])
  if test $gl_cv_func_totalordermagf_incompatible = yes; then
    REPLACE_TOTALORDERMAGF=1
  else
    gl_MATHFUNC([totalordermagf], [int], [(float const *, float const *)])
    if test $gl_cv_func_totalordermagf_no_libm != yes \
       && test $gl_cv_func_totalordermagf_in_libm != yes; then
      HAVE_TOTALORDERMAGF=0
    fi
  fi
  if test $HAVE_TOTALORDERMAGF = 0 || test $REPLACE_TOTALORDERMAGF = 1; then
    TOTALORDERMAGF_LIBM='$(ISNANF_LIBM)'
    dnl Prerequisite of lib/totalordermagf.c.
    gl_FLOAT_SIGN_LOCATION
    gl_NAN_MIPS
  fi
  AC_SUBST([TOTALORDERMAGF_LIBM])
])

AC_DEFUN([gl_FUNC_TOTALORDERMAG],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])

  dnl glibc versions < 2.31 had an incompatible declaration of this function,
  dnl see <https://sourceware.org/git/?p=glibc.git;a=commitdiff;h=42760d764649ad82f5fe45a26cbdf2c2500409f7>
  AC_CACHE_CHECK([whether totalordermag has a non-standard declaration],
    [gl_cv_func_totalordermag_incompatible],
    [AC_COMPILE_IFELSE(
       [AC_LANG_PROGRAM(
          [[#include <math.h>
          ]],
          [[extern
            #ifdef __cplusplus
            "C"
            #endif
            int totalordermag (double const *, double const *);
          ]])
       ],
       [gl_cv_func_totalordermag_incompatible=no],
       [gl_cv_func_totalordermag_incompatible=yes])
    ])
  if test $gl_cv_func_totalordermag_incompatible = yes; then
    REPLACE_TOTALORDERMAG=1
  else
    gl_MATHFUNC([totalordermag], [int], [(double const *, double const *)])
    if test $gl_cv_func_totalordermag_no_libm != yes \
       && test $gl_cv_func_totalordermag_in_libm != yes; then
      HAVE_TOTALORDERMAG=0
    fi
  fi
  if test $HAVE_TOTALORDERMAG = 0 || test $REPLACE_TOTALORDERMAG = 1; then
    TOTALORDERMAG_LIBM='$(ISNAND_LIBM)'
    dnl Prerequisite of lib/totalordermag.c.
    gl_DOUBLE_SIGN_LOCATION
    gl_NAN_MIPS
  fi
  AC_SUBST([TOTALORDERMAG_LIBM])
])

AC_DEFUN([gl_FUNC_TOTALORDERMAGL],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])
  AC_REQUIRE([gl_LONG_DOUBLE_VS_DOUBLE])

  dnl glibc versions < 2.31 had an incompatible declaration of this function,
  dnl see <https://sourceware.org/git/?p=glibc.git;a=commitdiff;h=42760d764649ad82f5fe45a26cbdf2c2500409f7>
  AC_CACHE_CHECK([whether totalordermagl has a non-standard declaration],
    [gl_cv_func_totalordermagl_incompatible],
    [AC_COMPILE_IFELSE(
       [AC_LANG_PROGRAM(
          [[#include <math.h>
          ]],
          [[extern
            #ifdef __cplusplus
            "C"
            #endif
            int totalordermagl (long double const *, long double const *);
          ]])
       ],
       [gl_cv_func_totalordermagl_incompatible=no],
       [gl_cv_func_totalordermagl_incompatible=yes])
    ])
  if test $gl_cv_func_totalordermagl_incompatible = yes; then
    REPLACE_TOTALORDERMAGL=1
  else
    gl_MATHFUNC([totalordermagl], [int],
      [(long double const *, long double const *)])
    if test $gl_cv_func_totalordermagl_no_libm != yes \
       && test $gl_cv_func_totalordermagl_in_libm != yes; then
      HAVE_TOTALORDERMAGL=0
    fi
  fi
  if test $HAVE_TOTALORDERMAGL = 0 || test $REPLACE_TOTALORDERMAGL = 1; then
    dnl Find libraries needed to link lib/totalorderl.c.
    if test $HAVE_SAME_LONG_DOUBLE_AS_DOUBLE = 1; then
      AC_REQUIRE([gl_FUNC_TOTALORDERMAG])
      TOTALORDERMAGL_LIBM="$TOTALORDERMAG_LIBM"
    else
      TOTALORDERMAGL_LIBM='$(ISNANL_LIBM)'
    fi
    dnl Prerequisite of lib/totalordermagl.c.
    gl_LONG_DOUBLE_SIGN_LOCATION
    gl_NAN_MIPS
  fi
  AC_SUBST([TOTALORDERMAGL_LIBM])
])
