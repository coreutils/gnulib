# rintf.m4
# serial 5
dnl Copyright (C) 2011-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN([gl_FUNC_RINTF],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])

  dnl Persuade glibc <math.h> to declare rintf().
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])

  dnl Determine RINTF_LIBM.
  gl_MATHFUNC([rintf], [float], [(float)],
    [extern
     #ifdef __cplusplus
     "C"
     #endif
     float rintf (float);
    ])
  if test $gl_cv_func_rintf_no_libm = yes \
     || test $gl_cv_func_rintf_in_libm = yes; then
    HAVE_RINTF=1
  else
    HAVE_RINTF=0
    RINTF_LIBM=
  fi
  AC_SUBST([RINTF_LIBM])
])
