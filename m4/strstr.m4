# strstr.m4 serial 1
dnl Copyright (C) 2008 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl Check that strstr is efficient.
AC_DEFUN([gl_FUNC_STRSTR],
[
  AC_REQUIRE([gl_HEADER_STRING_H_DEFAULTS])
  AC_CACHE_CHECK([whether strstr works in linear time],
    [gl_cv_func_strstr_linear],
    [AC_RUN_IFELSE([AC_LANG_PROGRAM([
#include <string.h> /* for memmem */
#include <stdlib.h> /* for malloc */
#include <unistd.h> /* for alarm */
], [[size_t m = 1000000;
    char *haystack = (char *) malloc (2 * m + 2);
    char *needle = (char *) malloc (m + 2);
    void *result = 0;
    /* Failure to compile this test due to missing alarm is okay,
       since all such platforms (mingw) also have quadratic strstr.  */
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
	result = strstr (haystack, needle);
      }
    return !result;]])],
      [gl_cv_func_strstr_linear=yes], [gl_cv_func_strstr_linear=no],
      [dnl pessimistically assume the worst, since even glibc 2.6.1
       dnl has quadratic complexity in its strstr
       gl_cv_func_strstr_linear="guessing no"])])
  if test "$gl_cv_func_strstr_linear" != yes; then
    REPLACE_STRSTR=1
    AC_LIBOBJ([strstr])
  fi
]) # gl_FUNC_STRSTR
