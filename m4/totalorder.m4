# totalorder.m4
# serial 3
dnl Copyright 2023-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN([gl_FUNC_TOTALORDERF],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])

  dnl glibc versions < 2.31 had an incompatible declaration of this function,
  dnl see <https://sourceware.org/git/?p=glibc.git;a=commitdiff;h=42760d764649ad82f5fe45a26cbdf2c2500409f7>
  AC_CACHE_CHECK([whether totalorderf has a non-standard declaration],
    [gl_cv_func_totalorderf_incompatible],
    [AC_COMPILE_IFELSE(
       [AC_LANG_PROGRAM(
          [[#include <math.h>
          ]],
          [[extern
            #ifdef __cplusplus
            "C"
            #endif
            int totalorderf (float const *, float const *);
          ]])
       ],
       [gl_cv_func_totalorderf_incompatible=no],
       [gl_cv_func_totalorderf_incompatible=yes])
    ])
  if test $gl_cv_func_totalorderf_incompatible = yes; then
    REPLACE_TOTALORDERF=1
  else
    gl_MATHFUNC([totalorderf], [int], [(float const *, float const *)])
    if test $gl_cv_func_totalorderf_no_libm != yes \
       && test $gl_cv_func_totalorderf_in_libm != yes; then
      HAVE_TOTALORDERF=0
    fi
  fi
  if test $HAVE_TOTALORDERF = 0 || test $REPLACE_TOTALORDERF = 1; then
    TOTALORDERF_LIBM='$(ISNANF_LIBM)'
    dnl Prerequisite of lib/totalorderf.c.
    gl_NAN_MIPS
  fi
  AC_SUBST([TOTALORDERF_LIBM])
])

AC_DEFUN([gl_FUNC_TOTALORDER],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])

  dnl glibc versions < 2.31 had an incompatible declaration of this function,
  dnl see <https://sourceware.org/git/?p=glibc.git;a=commitdiff;h=42760d764649ad82f5fe45a26cbdf2c2500409f7>
  AC_CACHE_CHECK([whether totalorder has a non-standard declaration],
    [gl_cv_func_totalorder_incompatible],
    [AC_COMPILE_IFELSE(
       [AC_LANG_PROGRAM(
          [[#include <math.h>
          ]],
          [[extern
            #ifdef __cplusplus
            "C"
            #endif
            int totalorder (double const *, double const *);
          ]])
       ],
       [gl_cv_func_totalorder_incompatible=no],
       [gl_cv_func_totalorder_incompatible=yes])
    ])
  if test $gl_cv_func_totalorder_incompatible = yes; then
    REPLACE_TOTALORDER=1
  else
    gl_MATHFUNC([totalorder], [int], [(double const *, double const *)])
    if test $gl_cv_func_totalorder_no_libm != yes \
       && test $gl_cv_func_totalorder_in_libm != yes; then
      HAVE_TOTALORDER=0
    fi
  fi
  if test $HAVE_TOTALORDER = 0 || test $REPLACE_TOTALORDER = 1; then
    TOTALORDER_LIBM='$(ISNAND_LIBM)'
    dnl Prerequisite of lib/totalorder.c.
    gl_NAN_MIPS
  fi
  AC_SUBST([TOTALORDER_LIBM])
])

AC_DEFUN([gl_FUNC_TOTALORDERL],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])
  AC_REQUIRE([gl_LONG_DOUBLE_VS_DOUBLE])

  dnl glibc versions < 2.31 had an incompatible declaration of this function,
  dnl see <https://sourceware.org/git/?p=glibc.git;a=commitdiff;h=42760d764649ad82f5fe45a26cbdf2c2500409f7>
  AC_CACHE_CHECK([whether totalorderl has a non-standard declaration],
    [gl_cv_func_totalorderl_incompatible],
    [AC_COMPILE_IFELSE(
       [AC_LANG_PROGRAM(
          [[#include <math.h>
          ]],
          [[extern
            #ifdef __cplusplus
            "C"
            #endif
            int totalorderl (long double const *, long double const *);
          ]])
       ],
       [gl_cv_func_totalorderl_incompatible=no],
       [gl_cv_func_totalorderl_incompatible=yes])
    ])
  if test $gl_cv_func_totalorderl_incompatible = yes; then
    REPLACE_TOTALORDERL=1
  else
    gl_MATHFUNC([totalorderl], [int],
      [(long double const *, long double const *)])
    if test $gl_cv_func_totalorderl_no_libm != yes \
       && test $gl_cv_func_totalorderl_in_libm != yes; then
      HAVE_TOTALORDERL=0
    fi
  fi
  if test $HAVE_TOTALORDERL = 0 || test $REPLACE_TOTALORDERL = 1; then
    dnl Find libraries needed to link lib/totalorderl.c.
    if test $HAVE_SAME_LONG_DOUBLE_AS_DOUBLE = 1; then
      AC_REQUIRE([gl_FUNC_TOTALORDER])
      TOTALORDERL_LIBM="$TOTALORDER_LIBM"
    else
      TOTALORDERL_LIBM='$(ISNANL_LIBM)'
    fi
    dnl Prerequisite of lib/totalorderl.c.
    gl_LONG_DOUBLE_SIGN_LOCATION
    gl_NAN_MIPS
  fi
  AC_SUBST([TOTALORDERL_LIBM])
])
