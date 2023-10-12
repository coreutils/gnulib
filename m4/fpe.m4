# fpe.m4 serial 1
dnl Copyright (C) 2023 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

# Prerequisites for tracking FE_INVALID exceptions.
AC_DEFUN_ONCE([gl_FPE_TRACKING],
[
  AC_CACHE_CHECK([whether FE_INVALID exceptions can be tracked],
    [gl_cv_fenv_invalid_api],
    [AC_COMPILE_IFELSE(
       [AC_LANG_PROGRAM(
          [[#include <fenv.h>
          ]],
          [[feclearexcept (FE_INVALID);
          ]])
       ],
       [gl_cv_fenv_invalid_api=yes],
       [gl_cv_fenv_invalid_api=no])
    ])
  if test $gl_cv_fenv_invalid_api = yes; then
    AC_DEFINE([HAVE_FE_INVALID], [1],
      [Define if FE_INVALID exceptions can be programmatically tracked.])
    AC_CACHE_CHECK([whether feclearexcept can be used without linking with libm],
      [gl_cv_func_feclearexcept_no_libm],
      [AC_LINK_IFELSE(
         [AC_LANG_PROGRAM(
            [[#include <fenv.h>
            ]],
            [[feclearexcept (FE_INVALID);
            ]])
         ],
         [gl_cv_func_feclearexcept_no_libm=yes],
         [gl_cv_func_feclearexcept_no_libm=no])
      ])
    if test $gl_cv_func_feclearexcept_no_libm != yes; then
      FPE_TRACKING_LIBM=-lm
    else
      FPE_TRACKING_LIBM=
    fi
  else
    FPE_TRACKING_LIBM=
  fi
  AC_SUBST([FPE_TRACKING_LIBM])
])

# Prerequisites for turning FE_INVALID exceptions into a SIGFPE signal.
AC_DEFUN_ONCE([gl_FPE_TRAPPING],
[
  dnl Persuade glibc <fenv.h> to declare feenableexcept().
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])

  gl_MATHFUNC([feenableexcept], [int], [(int)], [#include <fenv.h>])
  if test $gl_cv_func_feenableexcept_no_libm = yes \
     || test $gl_cv_func_feenableexcept_in_libm = yes; then
    AC_DEFINE([HAVE_FEENABLEEXCEPT], [1],
      [Define to 1 if you have the 'feenableexcept' function.])
  fi

  gl_MATHFUNC([fpsetmask], [fp_except_t], [(fp_except_t)],
    [#include <ieeefp.h>
     /* The type is called 'fp_except_t' on FreeBSD, but 'fp_except' on
        all other systems.  */
     #if !defined __FreeBSD__
     #define fp_except_t fp_except
     #endif
    ])
  if test $gl_cv_func_fpsetmask_no_libm = yes \
     || test $gl_cv_func_fpsetmask_in_libm = yes; then
    AC_DEFINE([HAVE_FPSETMASK], [1],
      [Define to 1 if you have the 'fpsetmask' function.])
  fi

  gl_MATHFUNC([fesettrapenable], [int], [(int)], [#include <fenv.h>])
  if test $gl_cv_func_fesettrapenable_no_libm = yes \
     || test $gl_cv_func_fesettrapenable_in_libm = yes; then
    AC_DEFINE([HAVE_FESETTRAPENABLE], [1],
      [Define to 1 if you have the 'fesettrapenable' function.])
  fi

  gl_MUSL_LIBC

  if test $gl_cv_func_feenableexcept_no_libm = yes \
     || test $gl_cv_func_feenableexcept_in_libm = yes; then
    if test $gl_cv_func_feenableexcept_no_libm != yes; then
      FPE_TRAPPING_LIBM=-lm
    else
      FPE_TRAPPING_LIBM=
    fi
  else
    if test $gl_cv_func_fpsetmask_no_libm = yes \
       || test $gl_cv_func_fpsetmask_in_libm = yes; then
      if test $gl_cv_func_fpsetmask_no_libm != yes; then
        FPE_TRAPPING_LIBM=-lm
      else
        FPE_TRAPPING_LIBM=
      fi
    else
      if test $gl_cv_func_fesettrapenable_no_libm = yes \
         || test $gl_cv_func_fesettrapenable_in_libm = yes; then
        if test $gl_cv_func_fesettrapenable_no_libm != yes; then
          FPE_TRAPPING_LIBM=-lm
        else
          FPE_TRAPPING_LIBM=
        fi
      else
        FPE_TRAPPING_LIBM=
      fi
    fi
  fi
  AC_REQUIRE([gl_FPE_TRACKING])
  FPE_TRAPPING_LIBM="$FPE_TRAPPING_LIBM $FPE_TRACKING_LIBM"
  AC_SUBST([FPE_TRAPPING_LIBM])
])
