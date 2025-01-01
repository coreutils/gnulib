# logp1f.m4
# serial 1
dnl Copyright (C) 2024-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN([gl_FUNC_LOGP1F],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  AC_REQUIRE([gl_FUNC_LOGP1])

  dnl Persuade glibc <math.h> to declare logp1f().
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])

  dnl Test whether logp1f() exists. Assume that logp1f(), if it exists, is
  dnl defined in the same library as logp1().
  saved_LIBS="$LIBS"
  LIBS="$LIBS $LOGP1_LIBM"
  AC_CHECK_FUNCS([logp1f])
  LIBS="$saved_LIBS"
  if test $ac_cv_func_logp1f = yes; then
    LOGP1F_LIBM="$LOGP1_LIBM"
  else
    HAVE_LOGP1F=0
    dnl We implement logp1f through log1pf.
    AC_REQUIRE([gl_FUNC_LOG1PF])
    LOGP1F_LIBM="$LOG1PF_LIBM"
  fi
  AC_SUBST([LOGP1F_LIBM])
])
