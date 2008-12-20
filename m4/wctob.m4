# wctob.m4 serial 2
dnl Copyright (C) 2008 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_WCTOB],
[
  AC_REQUIRE([gl_WCHAR_H_DEFAULTS])

  AC_CHECK_FUNCS_ONCE([wctob])
  if test $ac_cv_func_wctob = no; then
    HAVE_DECL_WCTOB=0
    gl_REPLACE_WCHAR_H
    AC_LIBOBJ([wctob])
    gl_PREREQ_WCTOB
  else
    dnl IRIX 6.5 has the wctob() function but does not declare it.
    AC_CHECK_DECLS([wctob], [], [], [
/* Tru64 with Desktop Toolkit C has a bug: <stdio.h> must be included before
   <wchar.h>.
   BSD/OS 4.0.1 has a bug: <stddef.h>, <stdio.h> and <time.h> must be included
   before <wchar.h>.  */
#include <stddef.h>
#include <stdio.h>
#include <time.h>
#include <wchar.h>
])
    if test $ac_cv_have_decl_wctob != yes; then
      HAVE_DECL_WCTOB=0
      gl_REPLACE_WCHAR_H
    fi
  fi
])

# Prerequisites of lib/wctob.c.
AC_DEFUN([gl_PREREQ_WCTOB], [
  :
])
