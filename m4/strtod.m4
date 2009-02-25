# strtod.m4 serial 12
dnl Copyright (C) 2002-2003, 2006-2009 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_STRTOD],
[
  AC_REQUIRE([gl_STDLIB_H_DEFAULTS])
  AC_FUNC_STRTOD
  dnl Note: AC_FUNC_STRTOD does AC_LIBOBJ([strtod]).
  if test $ac_cv_func_strtod = no; then
    HAVE_STRTOD=0
    REPLACE_STRTOD=1
    gl_PREREQ_STRTOD
  else
    AC_CACHE_CHECK([whether strtod obeys C99], [gl_cv_func_strtod_works],
      [AC_RUN_IFELSE([AC_LANG_PROGRAM([[
#include <stdlib.h>
#include <math.h>
#include <errno.h>
/* Compare two numbers with ==.
   This is a separate function because IRIX 6.5 "cc -O" miscompiles an
   'x == x' test.  */
static int
numeric_equal (double x, double y)
{
  return x == y;
}
]], [[
  {
    /* Older glibc and Cygwin mis-parse "-0x".  */
    const char *string = "-0x";
    char *term;
    double value = strtod (string, &term);
    double zero = 0.0;
    if (1.0 / value != -1.0 / zero || term != (string + 2))
      return 1;
  }
  {
    /* Many platforms do not parse hex floats.  */
    const char *string = "0XaP+1";
    char *term;
    double value = strtod (string, &term);
    if (value != 20.0 || term != (string + 6))
      return 1;
  }
  {
    /* Many platforms do not parse infinities.  HP-UX 11.31 parses inf,
       but mistakenly sets errno.  */
    const char *string = "inf";
    char *term;
    double value;
    errno = 0;
    value = strtod (string, &term);
    if (value != HUGE_VAL || term != (string + 3) || errno)
      return 1;
  }
  {
    /* glibc 2.7 and cygwin 1.5.24 misparse "nan()".  */
    const char *string = "nan()";
    char *term;
    double value = strtod (string, &term);
    if (numeric_equal (value, value) || term != (string + 5))
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
