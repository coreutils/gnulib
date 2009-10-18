# nproc.m4 serial 2
dnl Copyright (C) 2009 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_NPROC],
[
  gl_PREREQ_NPROC
])

# Prerequisites of lib/nproc.c.
AC_DEFUN([gl_PREREQ_NPROC],
[
  AC_CHECK_HEADERS([sys/pstat.h sys/param.h],,, [AC_INCLUDES_DEFAULT])
  dnl <sys/sysctl.h> requires <sys/param.h> on OpenBSD 4.0.
  AC_CHECK_HEADERS([sys/sysctl.h],,,
    [AC_INCLUDES_DEFAULT
     #if HAVE_SYS_PARAM_H
     # include <sys/param.h>
     #endif
    ])
  AC_CHECK_FUNCS([pstat_getdynamic sysctl])
])
