# check-math-lib.m4 serial 1
dnl Copyright (C) 2007 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl
dnl AC_CHECK_MATH_LIB (VARIABLE, EXPRESSION)
dnl
dnl Checks whether EXPRESSION requires -lm to compile and link.  If so, sets
dnl the shell VARIABLE to -lm, otherwise to the empty string.
dnl
dnl Example: AC_CHECK_MATH_LIB([ROUNDF_LIBM], [x = roundf (x);])
AC_DEFUN([gl_CHECK_MATH_LIB], [
  save_LIBS=$LIBS
  $1=?
  for libm in "" "-lm"; do
    LIBS="$save_LIBS $libm"
    AC_TRY_LINK([
       #ifndef __NO_MATH_INLINES
       # define __NO_MATH_INLINES 1 /* for glibc */
       #endif
       #include <math.h>
       double x;],
      [$2],
      [$1=$libm
break])
  done
  LIBS=$save_LIBS
  if test "$$1" = "?"; then
    $1=
  fi
])
