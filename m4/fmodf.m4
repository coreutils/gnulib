# fmodf.m4
# serial 15
dnl Copyright (C) 2011-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN([gl_FUNC_FMODF],
[
  m4_divert_text([DEFAULTS], [gl_fmodf_required=plain])
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  AC_REQUIRE([gl_FUNC_FMOD])

  dnl Persuade glibc <math.h> to declare fmodf().
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])

  dnl Test whether fmodf() exists. Assume that fmodf(), if it exists, is
  dnl defined in the same library as fmod().
  saved_LIBS="$LIBS"
  LIBS="$LIBS $FMOD_LIBM"
  AC_CHECK_FUNCS([fmodf])
  LIBS="$saved_LIBS"
  if test $ac_cv_func_fmodf = yes; then
    FMODF_LIBM="$FMOD_LIBM"
  else
    HAVE_FMODF=0
    dnl If the function is declared but does not appear to exist, it may be
    dnl defined as an inline function. In order to avoid a conflict, we have
    dnl to define rpl_fmodf, not fmodf.
    AC_CHECK_DECLS([fmodf], [REPLACE_FMODF=1], , [[#include <math.h>]])
  fi
  if test $HAVE_FMODF = 0 || test $REPLACE_FMODF = 1; then
    dnl Find libraries needed to link lib/fmodf.c.
    FMODF_LIBM="$FMOD_LIBM"
  fi
  AC_SUBST([FMODF_LIBM])
])
