#serial 8

# Copyright (C) 2001, 2002, 2003, 2005, 2007 Free Software Foundation, Inc.
# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_GETTIMEOFDAY],
[
  AC_REQUIRE([AC_C_RESTRICT])
  AC_REQUIRE([AC_HEADER_TIME])
  AC_CHECK_HEADERS_ONCE([sys/time.h])
  AC_CHECK_FUNCS([gettimeofday])
  
  AC_CHECK_TYPE([suseconds_t], ,
    [AC_DEFINE([suseconds_t], [int],
       [Define to `int' if `suseconds_t' is missing.])
    ],
    [
#    if TIME_WITH_SYS_TIME
#     include <sys/time.h>
#     include <time.h>
#    else
#     if HAVE_SYS_TIME_H
#      include <sys/time.h>
#     else
#      include <time.h>
#     endif
#    endif
    ])

  AC_CACHE_CHECK([for struct timeval], [fu_cv_sys_struct_timeval],
    [AC_TRY_COMPILE(
      [
#      if TIME_WITH_SYS_TIME
#       include <sys/time.h>
#       include <time.h>
#      else
#       if HAVE_SYS_TIME_H
#        include <sys/time.h>
#       else
#        include <time.h>
#       endif
#      endif
      ],
      [static struct timeval x; x.tv_sec = x.tv_usec;],
      fu_cv_sys_struct_timeval=yes,
      fu_cv_sys_struct_timeval=no)
    ])

  if test $fu_cv_sys_struct_timeval = yes; then
    AC_DEFINE(HAVE_STRUCT_TIMEVAL, 1,
      [Define if struct timeval is declared in <time.h> or <sys/time.h>. ])
  fi
  
  AC_CACHE_CHECK([for gettimeofday whose signature conforms to POSIX],
    [ac_cv_func_gettimeofday_posix_signature],
    [AC_LINK_IFELSE(
       [AC_LANG_PROGRAM(
          [[#include <sys/time.h>
	    time_t a;
	    suseconds_t b;
	    struct timeval c;
	  ]],
	  [[
	    int x = gettimeofday (&c, 0);
	    int (*f) (struct timeval *restrict, void *restrict) = gettimeofday;
	    return !(x | c.tv_sec | c.tv_usec);
	  ]])],
       [ac_cv_func_gettimeofday_posix_signature=yes],
       [ac_cv_func_gettimeofday_posix_signature=no])])
  if test $ac_cv_func_gettimeofday_posix_signature = yes; then
    AC_DEFINE([HAVE_GETTIMEOFDAY_POSIX_SIGNATURE], 1,
      [Define if gettimeofday's signature conforms to POSIX.])
    AC_FUNC_GETTIMEOFDAY_CLOBBER
  else
    gl_PREREQ_GETTIMEOFDAY
    AC_LIBOBJ([gettimeofday])
  fi
])

dnl From Jim Meyering.
dnl
dnl See if gettimeofday clobbers the static buffer that localtime uses
dnl for its return value.  The gettimeofday function from Mac OS X 10.0.4
dnl (i.e., Darwin 1.3.7) has this problem.
dnl
dnl If it does, then arrange to use gettimeofday and localtime only via
dnl the wrapper functions that work around the problem.

AC_DEFUN([AC_FUNC_GETTIMEOFDAY_CLOBBER],
[
 AC_REQUIRE([AC_HEADER_TIME])
 AC_CACHE_CHECK([whether gettimeofday clobbers localtime buffer],
  [jm_cv_func_gettimeofday_clobber],
  [AC_TRY_RUN([
#include <stdio.h>
#include <string.h>

#if TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif

#include <stdlib.h>

int
main ()
{
  time_t t = 0;
  struct tm *lt;
  struct tm saved_lt;
  struct timeval tv;
  lt = localtime (&t);
  saved_lt = *lt;
  gettimeofday (&tv, NULL);
  if (memcmp (lt, &saved_lt, sizeof (struct tm)) != 0)
    exit (1);

  exit (0);
}
	  ],
	 jm_cv_func_gettimeofday_clobber=no,
	 jm_cv_func_gettimeofday_clobber=yes,
	 dnl When crosscompiling, assume it is broken.
	 jm_cv_func_gettimeofday_clobber=yes)
  ])
  if test $jm_cv_func_gettimeofday_clobber = yes; then
    gl_GETTIMEOFDAY_REPLACE_LOCALTIME
  fi
])

AC_DEFUN([gl_GETTIMEOFDAY_REPLACE_LOCALTIME], [
  AC_LIBOBJ(gettimeofday)
  gl_PREREQ_GETTIMEOFDAY
  AC_DEFINE([GETTIMEOFDAY_CLOBBERS_LOCALTIME], 1,
    [Define if gettimeofday clobbers the localtime buffer.])
  AC_DEFINE([gmtime], [rpl_gmtime],
    [Define to rpl_gmtime if the replacement function should be used.])
  AC_DEFINE([localtime], [rpl_localtime],
    [Define to rpl_localtime if the replacement function should be used.])
  AC_DEFINE([tzset], [rpl_tzset],
    [Define to rpl_tzset if the replacement function should be used.])
])

# Prerequisites of lib/gettimeofday.c.
AC_DEFUN([gl_PREREQ_GETTIMEOFDAY], [
  AC_REQUIRE([AC_HEADER_TIME])
  AC_CHECK_HEADERS([sys/timeb.h])
  AC_CHECK_FUNCS([_ftime])
])
