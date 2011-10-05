# fmodf.m4 serial 1
dnl Copyright (C) 2011 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_FMODF],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  AC_REQUIRE([gl_FUNC_FMOD])

  dnl Test whether fmodf() exists. Assume that fmodf(), if it exists, is
  dnl defined in the same library as fmod().
  save_LIBS="$LIBS"
  LIBS="$LIBS $FMOD_LIBM"
  AC_CHECK_FUNCS([fmodf])
  LIBS="$save_LIBS"
  if test $ac_cv_func_fmodf = yes; then
    FMODF_LIBM="$FMOD_LIBM"
  else
    HAVE_FMODF=0
    FMODF_LIBM="$FMOD_LIBM"
  fi
  AC_SUBST([FMODF_LIBM])
])
