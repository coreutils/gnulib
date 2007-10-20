# round.m4 serial 1
dnl Copyright (C) 2007 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_ROUND],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  dnl Persuade glibc <math.h> to declare round().
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])
  gl_CHECK_LIBM_FUNC([round], [x = round(x);], [], [
    AC_REQUIRE([gl_FUNC_FLOOR])
    ROUND_LIBM=$FLOOR_LIBM
    AC_LIBOBJ([round])])])
