# fenv-exceptions.m4
# serial 1
dnl Copyright (C) 2023-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

dnl Prerequisites of most lib/fenv-except-*.c files.
AC_DEFUN([gl_PREREQ_FENV_EXCEPTIONS],
[
  gl_MATHFUNC([fpsetsticky], [fp_except_t], [(fp_except_t)],
    [#include <ieeefp.h>
     /* The type is called 'fp_except_t' on FreeBSD, but 'fp_except' on
        all other systems.  */
     #if !defined __FreeBSD__
     #define fp_except_t fp_except
     #endif
    ])
  if test $gl_cv_func_fpsetsticky_no_libm = yes \
     || test $gl_cv_func_fpsetsticky_in_libm = yes; then
    AC_DEFINE([HAVE_FPSETSTICKY], [1],
      [Define to 1 if you have the 'fpsetsticky' function.])
  fi
])
