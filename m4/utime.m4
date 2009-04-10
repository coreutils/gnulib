# serial 9

dnl From Jim Meyering
dnl Replace the utime function on systems that need it.

# Copyright (C) 1998, 2000-2001, 2003-2004, 2009 Free Software Foundation, Inc.
# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

dnl FIXME

AC_DEFUN([gl_FUNC_UTIME],
[
  AC_REQUIRE([AC_FUNC_UTIME_NULL])
  if test $ac_cv_func_utime_null = no; then
    AC_LIBOBJ([utime])
    AC_DEFINE([utime], [rpl_utime],
      [Define to rpl_utime if the replacement function should be used.])
    gl_PREREQ_UTIME
  fi
])

# Prerequisites of lib/utime.c.
AC_DEFUN([gl_PREREQ_UTIME],
[
  AC_CHECK_HEADERS_ONCE([utime.h])
  AC_REQUIRE([gl_CHECK_TYPE_STRUCT_UTIMBUF])
  gl_FUNC_UTIMES_NULL
])

# Use the definition of AC_FUNC_UTIME_NULL from autoconf 2.64 or newer.
# Remove this macro when we can assume autoconf >= 2.64.
m4_version_prereq([2.64], [], [
AC_DEFUN([AC_FUNC_UTIME_NULL],
[AC_CHECK_HEADERS_ONCE([utime.h])
AC_CACHE_CHECK([whether utime accepts a null argument], [ac_cv_func_utime_null],
[rm -f conftest.data; >conftest.data
# Sequent interprets utime(file, 0) to mean use start of epoch.  Wrong.
AC_RUN_IFELSE([AC_LANG_PROGRAM([AC_INCLUDES_DEFAULT
	       #ifdef HAVE_UTIME_H
	       # include <utime.h>
	       #endif],
[[struct stat s, t;
  return ! (stat ("conftest.data", &s) == 0
	    && utime ("conftest.data", 0) == 0
	    && stat ("conftest.data", &t) == 0
	    && t.st_mtime >= s.st_mtime
	    && t.st_mtime - s.st_mtime < 120);]])],
	      ac_cv_func_utime_null=yes,
	      ac_cv_func_utime_null=no,
	      ac_cv_func_utime_null='guessing yes')])
if test "x$ac_cv_func_utime_null" != xno; then
  ac_cv_func_utime_null=yes
  AC_DEFINE([HAVE_UTIME_NULL], [1],
	    [Define to 1 if `utime(file, NULL)' sets file's timestamp to the
	     present.])
fi
rm -f conftest.data
])
])
