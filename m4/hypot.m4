# hypot.m4 serial 1
dnl Copyright (C) 2012 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_HYPOT],
[
  dnl Determine HYPOT_LIBM.
  gl_COMMON_DOUBLE_MATHFUNC([hypot])
])
