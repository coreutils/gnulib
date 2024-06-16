# tm_gmtoff.m4
# serial 4
dnl Copyright (C) 2002, 2009-2024 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_TM_GMTOFF],
[
  AC_CHECK_MEMBERS([struct tm.tm_gmtoff], [], [],
    [[#include <time.h>
    ]])

  dnl Backward compatibility with 2024-and-earlier versions of this macro.
  AS_IF([test "$ac_cv_member_struct_tm_tm_gmtoff" = yes],
    [AC_DEFINE([HAVE_TM_GMTOFF], [1],
       [Define if struct tm has the tm_gmtoff member.
        This macro is obsolete.
        New code should use HAVE_STRUCT_TM_TM_GMTOFF.])])
])
