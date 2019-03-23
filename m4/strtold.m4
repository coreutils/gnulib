# strtold.m4 serial 4
dnl Copyright (C) 2002-2003, 2006-2019 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_STRTOLD],
[
  AC_REQUIRE([gl_STDLIB_H_DEFAULTS])
  AC_REQUIRE([gl_LONG_DOUBLE_VS_DOUBLE])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  AC_CHECK_FUNCS_ONCE([strtold])
  if test $ac_cv_func_strtold != yes; then
    HAVE_STRTOLD=0
  else
    AC_CACHE_CHECK([whether strtold obeys C99], [gl_cv_func_strtold_works],
      [AC_RUN_IFELSE([AC_LANG_PROGRAM([[
#include <stdlib.h>
#include <math.h>
#include <errno.h>
/* Compare two numbers with ==.
   This is a separate function because IRIX 6.5 "cc -O" miscompiles an
   'x == x' test.  */
static int
numeric_equal (long double x, long double y)
{
  return x == y;
}
]], [[
  int result = 0;
  {
    /* Under Solaris 2.4, strtod returns the wrong value for the
       terminating character under some conditions.  */
    const char *string = "NaN";
    char *term;
    strtod (string, &term);
    if (term != string && *(term - 1) == 0)
      result |= 2;
  }
  {
    /* Older glibc and Cygwin mis-parse "-0x".  */
    const char *string = "-0x";
    char *term;
    long double value = strtold (string, &term);
    long double zero = 0.0L;
    if (1.0L / value != -1.0L / zero || term != (string + 2))
      result |= 4;
  }
  {
    /* IRIX 6.5, mingw do not parse hex floats.  */
    const char *string = "0XaP+1";
    char *term;
    long double value = strtold (string, &term);
    if (value != 20.0L || term != (string + 6))
      result |= 8;
  }
  {
    /* IRIX 6.5 does not parse infinities.  HP-UX 11.31/ia64 parses inf,
       but mistakenly sets errno.  */
    const char *string = "inf";
    char *term;
    long double value;
    errno = 0;
    value = strtod (string, &term);
    if (value != HUGE_VAL || term != (string + 3) || errno)
      result |= 16;
  }
  {
    /* glibc-2.3.2, IRIX 6.5, mingw, Haiku misparse "nan()".  */
    const char *string = "nan()";
    char *term;
    long double value = strtold (string, &term);
    if (numeric_equal (value, value) || term != (string + 5))
      result |= 32;
  }
  {
    /* Mac OS X 10.5, IRIX 6.5 misparse "nan(".  */
    const char *string = "nan(";
    char *term;
    long double value = strtold (string, &term);
    if (numeric_equal (value, value) || term != (string + 3))
      result |= 64;
  }
  return result;
]])],
        [gl_cv_func_strtold_works=yes],
        [gl_cv_func_strtold_works=no],
        [dnl The last known bugs in glibc strtold(), as of this writing,
         dnl were fixed in version 2.8
         AC_EGREP_CPP([Lucky user],
           [
#include <features.h>
#ifdef __GNU_LIBRARY__
 #if ((__GLIBC__ == 2 && __GLIBC_MINOR__ >= 8) || (__GLIBC__ > 2)) \
     && !defined __UCLIBC__
  Lucky user
 #endif
#endif
           ],
           [gl_cv_func_strtold_works="guessing yes"],
           [case "$host_os" in
                       # Guess yes on musl systems.
              *-musl*) gl_cv_func_strtold_works="guessing yes" ;;
              *)       gl_cv_func_strtold_works="guessing no" ;;
            esac
           ])
        ])
      ])
    case "$gl_cv_func_strtold_works" in
      *yes) ;;
      *)
        REPLACE_STRTOLD=1
        ;;
    esac
  fi
])

# Prerequisites of lib/strtold.c.
AC_DEFUN([gl_PREREQ_STRTOLD], [
  AC_REQUIRE([gl_CHECK_LDEXPL_NO_LIBM])
  if test $gl_cv_func_ldexpl_no_libm = yes; then
    AC_DEFINE([HAVE_LDEXPL_IN_LIBC], [1],
      [Define if the ldexpl function is available in libc.])
  fi
  AC_CHECK_FUNCS([nl_langinfo])
])
