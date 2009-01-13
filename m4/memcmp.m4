# memcmp.m4 serial 14
dnl Copyright (C) 2002-2004, 2007-2009 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_MEMCMP],
[
  if test $cross_compiling != no; then
    # AC_FUNC_MEMCMP as of 2.62 defaults to 'no' when cross compiling.
    # We default to yes if memcmp appears to exist, which works
    # better for MinGW.
    AC_CACHE_CHECK([whether cross-compiling target has memcmp],
                   [ac_cv_func_memcmp_working],
                   [AC_LINK_IFELSE([
			AC_LANG_PROGRAM([[#include <string.h>
				]], [[int ret = memcmp ("foo", "bar", 0);]])],
			[ac_cv_func_memcmp_working=yes],
			[ac_cv_func_memcmp_working=no])])
  fi
  AC_FUNC_MEMCMP
  dnl Note: AC_FUNC_MEMCMP does AC_LIBOBJ([memcmp]).
  if test $ac_cv_func_memcmp_working = no; then
    AC_DEFINE([memcmp], [rpl_memcmp],
      [Define to rpl_memcmp if the replacement function should be used.])
    gl_PREREQ_MEMCMP
  fi
])

# Prerequisites of lib/memcmp.c.
AC_DEFUN([gl_PREREQ_MEMCMP], [:])
