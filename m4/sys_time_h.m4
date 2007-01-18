# Configure a replacement for <sys/time.h>.

# Copyright (C) 2007 Free Software Foundation, Inc.
# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# Written by Paul Eggert and Martin Lambers.

AC_DEFUN([gl_HEADER_SYS_TIME_H],
[
  AC_REQUIRE([AC_C_RESTRICT])
  AC_CHECK_HEADERS_ONCE([sys/time.h])

  if test $ac_cv_header_sys_time_h = yes; then
    gl_ABSOLUTE_HEADER([sys/time.h])
    ABSOLUTE_SYS_TIME_H=\"$gl_cv_absolute_sys_time_h\"
    HAVE_SYS_TIME_H=1
  else
    ABSOLUTE_SYS_TIME_H=\"no/such/file/sys/time.h\"
    HAVE_SYS_TIME_H=0
  fi
  AC_SUBST([ABSOLUTE_SYS_TIME_H])
  AC_SUBST([HAVE_SYS_TIME_H])

  AC_CACHE_CHECK([for struct timeval], [gl_cv_sys_struct_timeval],
    [AC_COMPILE_IFELSE(
       [AC_LANG_PROGRAM(
	  [[#if HAVE_SYS_TIME_H
	     #include <sys/time.h>
	    #endif
	    #include <time.h>
	  ]],
	  [[static struct timeval x; x.tv_sec = x.tv_usec;]])],
       [gl_cv_sys_struct_timeval=yes],
       [gl_cv_sys_struct_timeval=no])])
  if test $gl_cv_sys_struct_timeval = yes; then
    HAVE_STRUCT_TIMEVAL=1
  else
    HAVE_STRUCT_TIMEVAL=0
  fi
  AC_SUBST([HAVE_STRUCT_TIMEVAL])

  dnl Assume POSIX behavior unless another module says otherwise.
  HAVE_GETTIMEOFDAY_POSIX_SIGNATURE=1
  AC_SUBST([HAVE_GETTIMEOFDAY_POSIX_SIGNATURE])
  GETTIMEOFDAY_CLOBBERS_LOCALTIME=0
  AC_SUBST([GETTIMEOFDAY_CLOBBERS_LOCALTIME])
])
