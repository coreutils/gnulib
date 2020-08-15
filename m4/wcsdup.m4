# wcsdup.m4 serial 3
dnl Copyright (C) 2011-2020 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_WCSDUP],
[
  AC_REQUIRE([gl_WCHAR_H_DEFAULTS])
  AC_CACHE_CHECK([for wcsdup],
    [gl_cv_func_wcsdup],
    [AC_LINK_IFELSE(
       [AC_LANG_PROGRAM(
          [[
/* Tru64 with Desktop Toolkit C has a bug: <stdio.h> must be included before
   <wchar.h>.
   BSD/OS 4.0.1 has a bug: <stddef.h>, <stdio.h> and <time.h> must be included
   before <wchar.h>.  */
#include <stddef.h>
#include <stdio.h>
#include <time.h>
#include <wchar.h>
          ]GL_MDA_DEFINES],
          [[return wcsdup (L"hello") != NULL;]])
       ],
       [gl_cv_func_wcsdup=yes],
       [gl_cv_func_wcsdup=no])
    ])
  if test $gl_cv_func_wcsdup = no; then
    HAVE_WCSDUP=0
  fi
])
