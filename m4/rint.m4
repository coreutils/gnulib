# rint.m4 serial 1
dnl Copyright (C) 2011 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_RINT],
[
  dnl Determine RINT_LIBM.
  gl_MATHFUNC([rint], [double], [(double)])
  if test $gl_cv_func_rint_no_libm = no \
     && test $gl_cv_func_rint_in_libm = no; then
    HAVE_RINT=0
    RINT_LIBM=
  fi
  AC_SUBST([RINT_LIBM])
])
