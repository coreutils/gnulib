# strcasestr.m4 serial 9
dnl Copyright (C) 2005, 2007, 2008 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl Check that strcasestr is present.
AC_DEFUN([gl_FUNC_STRCASESTR_SIMPLE],
[
  dnl Persuade glibc <string.h> to declare strcasestr().
  AC_REQUIRE([AC_USE_SYSTEM_EXTENSIONS])

  AC_REQUIRE([gl_HEADER_STRING_H_DEFAULTS])
  AC_REPLACE_FUNCS([strcasestr])
  if test $ac_cv_func_strcasestr = no; then
    HAVE_STRCASESTR=0
    gl_PREREQ_STRCASESTR
  fi
]) # gl_FUNC_STRCASESTR_SIMPLE

dnl Additionally, check that strcasestr is efficient.
AC_DEFUN([gl_FUNC_STRCASESTR],
[
  AC_REQUIRE([gl_FUNC_STRCASESTR_SIMPLE])
  if test $ac_cv_func_strcasestr = yes; then
    AC_CACHE_CHECK([whether strcasestr works in linear time],
      [gl_cv_func_strcasestr_linear],
      [AC_RUN_IFELSE([AC_LANG_PROGRAM([[
#include <string.h> /* for memmem */
#include <stdlib.h> /* for malloc */
#include <unistd.h> /* for alarm */
]], [[size_t m = 1000000;
    char *haystack = (char *) malloc (2 * m + 2);
    char *needle = (char *) malloc (m + 2);
    void *result = 0;
    /* Failure to compile this test due to missing alarm is okay,
       since all such platforms (mingw) also lack strcasestr.  */
    alarm (5);
    /* Check for quadratic performance.  */
    if (haystack && needle)
      {
	memset (haystack, 'A', 2 * m);
	haystack[2 * m] = 'B';
	haystack[2 * m + 1] = 0;
	memset (needle, 'A', m);
	needle[m] = 'B';
	needle[m + 1] = 0;
	result = strcasestr (haystack, needle);
      }
    return !result;]])],
      [gl_cv_func_strcasestr_linear=yes], [gl_cv_func_strcasestr_linear=no],
      [dnl pessimistically assume the worst, since even glibc 2.6.1
       dnl has quadratic complexity in its strcasestr
       gl_cv_func_strcasestr_linear="guessing no"])])
    if test "$gl_cv_func_strcasestr_linear" != yes; then
      REPLACE_STRCASESTR=1
      AC_LIBOBJ([strcasestr])
    fi
  fi
]) # gl_FUNC_STRCASESTR

# Prerequisites of lib/strcasestr.c.
AC_DEFUN([gl_PREREQ_STRCASESTR], [
  :
])
