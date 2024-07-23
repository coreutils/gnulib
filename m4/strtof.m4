# strtof.m4
# serial 2
dnl Copyright (C) 2002-2003, 2006-2024 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_STRTOF],
[
  AC_REQUIRE([gl_STDLIB_H_DEFAULTS])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  dnl Test whether strtof is declared.
  dnl Don't call AC_FUNC_STRTOF, because it does not have the right guess
  dnl when cross-compiling.
  dnl Don't call AC_CHECK_FUNCS([strtof]) because it would collide with the
  dnl ac_cv_func_strtof variable set by the AC_FUNC_STRTOF macro.
  AC_CHECK_DECLS_ONCE([strtof])
  if test $ac_cv_have_decl_strtof != yes; then
    HAVE_STRTOF=0
  fi
  if test $HAVE_STRTOF = 1; then
    AC_DEFINE([HAVE_STRTOF], [1],
      [Define to 1 if you have the 'strtof' function.])
    AC_CACHE_CHECK([whether strtof obeys C99], [gl_cv_func_strtof_works],
      [AC_RUN_IFELSE([AC_LANG_PROGRAM([[
#include <stdlib.h>
#include <math.h>
#include <errno.h>
/* Compare two numbers with ==.
   This is a separate function because IRIX 6.5 "cc -O" miscompiles an
   'x == x' test.  */
static int
numeric_equal (float x, float y)
{
  return x == y;
}
]], [[
  int result = 0;
  {
    /* In some old versions of Linux (2000 or before), strtof mis-parses
       strings with leading '+'.  */
    const char *string = " +69";
    char *term;
    float value = strtof (string, &term);
    if (value != 69 || term != (string + 4))
      result |= 1;
  }
  {
    /* Under Solaris 2.4, strtof returns the wrong value for the
       terminating character under some conditions.  */
    const char *string = "NaN";
    char *term;
    strtof (string, &term);
    if (term != string && *(term - 1) == 0)
      result |= 2;
  }
  {
    /* Older glibc and Cygwin mis-parse "-0x".  */
    const char *string = "-0x";
    char *term;
    float value = strtof (string, &term);
    float zero = 0.0f;
    if (1.0f / value != -1.0f / zero || term != (string + 2))
      result |= 4;
  }
  {
    /* Many platforms do not parse hex floats.  */
    const char *string = "0XaP+1";
    char *term;
    float value = strtof (string, &term);
    if (value != 20.0f || term != (string + 6))
      result |= 8;
  }
  {
    /* Many platforms do not parse infinities.  HP-UX 11.31 parses inf,
       but mistakenly sets errno.  */
    const char *string = "inf";
    char *term;
    float value;
    errno = 0;
    value = strtof (string, &term);
    if (value != HUGE_VAL || term != (string + 3) || errno)
      result |= 16;
  }
  {
    /* glibc 2.7 and cygwin 1.5.24 misparse "nan()".  */
    const char *string = "nan()";
    char *term;
    float value = strtof (string, &term);
    if (numeric_equal (value, value) || term != (string + 5))
      result |= 32;
  }
  {
    /* darwin 10.6.1 misparses "nan(".  */
    const char *string = "nan(";
    char *term;
    float value = strtof (string, &term);
    if (numeric_equal (value, value) || term != (string + 3))
      result |= 64;
  }
  return result;
]])],
        [gl_cv_func_strtof_works=yes],
        [gl_cv_func_strtof_works=no],
        [dnl The last known bugs in glibc strtof(), as of this writing,
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
           [gl_cv_func_strtof_works="guessing yes"],
           [case "$host_os" in
                                  # Guess yes on musl systems.
              *-musl* | midipix*) gl_cv_func_strtof_works="guessing yes" ;;
                                  # Guess yes on native Windows.
              mingw* | windows*)  gl_cv_func_strtof_works="guessing yes" ;;
              *)                  gl_cv_func_strtof_works="$gl_cross_guess_normal" ;;
            esac
           ])
        ])
      ])
    case "$gl_cv_func_strtof_works" in
      *yes) ;;
      *)
        REPLACE_STRTOF=1
        ;;
    esac
  fi
])

# Prerequisites of lib/strtof.c.
AC_DEFUN([gl_PREREQ_STRTOF], [
  AC_REQUIRE([gl_CHECK_LDEXPF_NO_LIBM])
  if test $gl_cv_func_ldexpf_no_libm = yes; then
    AC_DEFINE([HAVE_LDEXPF_IN_LIBC], [1],
      [Define if the ldexpf function is available in libc.])
  fi
  gl_CHECK_FUNCS_ANDROID([nl_langinfo], [[#include <langinfo.h>]])
])
