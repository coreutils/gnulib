# rpmatch.m4 serial 6
dnl Copyright (C) 2002-2003, 2007-2008 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_RPMATCH],
[
  AC_REPLACE_FUNCS(rpmatch)
  if test $ac_cv_func_rpmatch = no; then
    gl_PREREQ_RPMATCH
  fi
])

# Prerequisites of lib/rpmatch.c.
AC_DEFUN([gl_PREREQ_RPMATCH], [
  AC_CACHE_CHECK([for nl_langinfo and YESEXPR], gl_cv_langinfo_yesexpr,
    [AC_TRY_LINK([#include <langinfo.h>],
       [char* cs = nl_langinfo(YESEXPR); return !cs;],
       [gl_cv_langinfo_yesexpr=yes],
       [gl_cv_langinfo_yesexpr=no])
    ])
  if test $gl_cv_langinfo_yesexpr = yes; then
    AC_DEFINE([HAVE_LANGINFO_YESEXPR], 1,
      [Define if you have <langinfo.h> and nl_langinfo(YESEXPR).])
  fi
])
