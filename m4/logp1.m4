# logp1.m4
# serial 1
dnl Copyright (C) 2024-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN([gl_FUNC_LOGP1],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])

  dnl Persuade glibc <math.h> to declare logp1().
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])

  dnl Determine LOGP1_LIBM.
  gl_COMMON_DOUBLE_MATHFUNC([logp1])

  dnl Test whether logp1() exists.
  saved_LIBS="$LIBS"
  LIBS="$LIBS $LOGP1_LIBM"
  AC_CHECK_FUNCS([logp1])
  LIBS="$saved_LIBS"
  if test $ac_cv_func_logp1 = no; then
    HAVE_LOGP1=0
    dnl We implement logp1 through log1p.
    AC_REQUIRE([gl_FUNC_LOG1P])
    LOGP1_LIBM="$LOG1P_LIBM"
  fi
  AC_SUBST([LOGP1_LIBM])
])
