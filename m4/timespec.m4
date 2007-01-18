#serial 13

# Copyright (C) 2000, 2001, 2003, 2004, 2005, 2006, 2007 Free Software
# Foundation, Inc.

# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

dnl From Jim Meyering

AC_DEFUN([gl_TIMESPEC],
[
  dnl Prerequisites of lib/timespec.h.
  AC_REQUIRE([AC_C_INLINE])
  AC_CHECK_HEADERS_ONCE(sys/time.h)
  gl_CHECK_TYPE_STRUCT_TIMESPEC

  dnl Persuade glibc and Solaris <time.h> to declare nanosleep.
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])

  AC_CHECK_DECLS(nanosleep, , , [#include <time.h>])
])

dnl Define HAVE_STRUCT_TIMESPEC if `struct timespec' is declared
dnl in time.h or sys/time.h.

AC_DEFUN([gl_CHECK_TYPE_STRUCT_TIMESPEC],
[
  dnl Persuade pedantic Solaris to declare struct timespec.
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])

  AC_CHECK_HEADERS_ONCE(sys/time.h)
  AC_CACHE_CHECK([for struct timespec], [gl_cv_sys_struct_timespec],
    [AC_COMPILE_IFELSE(
       [AC_LANG_PROGRAM(
	  [[#if HAVE_SYS_TIME_H
	     #include <sys/time.h>
	    #endif
	    #include <time.h>
	  ]],
	  [[static struct timespec x; x.tv_sec = x.tv_nsec;]])],
       [gl_cv_sys_struct_timespec=yes],
       [gl_cv_sys_struct_timespec=no])])

  if test $gl_cv_sys_struct_timespec = yes; then
    AC_DEFINE(HAVE_STRUCT_TIMESPEC, 1,
	      [Define if struct timespec is declared in <time.h>. ])
  fi
])
