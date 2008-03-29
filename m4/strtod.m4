# strtod.m4 serial 7
dnl Copyright (C) 2002, 2003, 2006, 2007, 2008 Free Software
dnl Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_STRTOD],
[
  AC_REQUIRE([gl_STDLIB_H_DEFAULTS])
  AC_FUNC_STRTOD
  dnl Note: AC_FUNC_STRTOD does AC_LIBOBJ(strtod).
  if test $ac_cv_func_strtod = no; then
    HAVE_STRTOD=0
    REPLACE_STRTOD=1
    gl_PREREQ_STRTOD
  else
    AC_CACHE_CHECK([whether strtod obeys C99], [gl_cv_func_strtod_works],
      [AC_RUN_IFELSE([AC_LANG_PROGRAM([[
#include <stdlib.h>
#include <math.h>
]], [[
  {
    /* Older glibc and Cygwin mis-parse "-0x".  */
    char *string = "-0x";
    char *term;
    double value = strtod (string, &term);
    if (1 / value != -HUGE_VAL || term != (string + 2))
      return 1;
  }
  {
    /* Many platforms do not parse infinities.  */
    char *string = "inf";
    char *term;
    double value = strtod (string, &term);
    if (value != HUGE_VAL || term != (string + 3))
      return 1;
  }
]])],
	[gl_cv_func_strtod_works=yes],
	[gl_cv_func_strtod_works=no],
	[gl_cv_func_strtod_works="guessing no"])])
    if test "$gl_cv_func_strtod_works" != yes; then
      REPLACE_STRTOD=1
      gl_PREREQ_STRTOD
      dnl Use undocumented macro to set POW_LIB correctly.
      _AC_LIBOBJ_STRTOD
    fi
  fi
])

# Prerequisites of lib/strtod.c.
# The need for pow() is already handled by AC_FUNC_STRTOD.
AC_DEFUN([gl_PREREQ_STRTOD], [:])
