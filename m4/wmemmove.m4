# wmemmove.m4 serial 3
dnl Copyright (C) 2011-2016 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_WMEMMOVE],
[
  AC_REQUIRE([gl_WCHAR_H_DEFAULTS])
  dnl We cannot use AC_CHECK_FUNCS here, because the MSVC 9 header files
  dnl provide this function as an inline function definition.
  AC_CACHE_CHECK([for wmemmove], [gl_cv_func_wmemmove],
    [AC_COMPILE_IFELSE(
       [AC_LANG_PROGRAM([[
/* Tru64 with Desktop Toolkit C has a bug: <stdio.h> must be included before
   <wchar.h>.
   BSD/OS 4.0.1 has a bug: <stddef.h>, <stdio.h> and <time.h> must be included
   before <wchar.h>.  */
#include <stddef.h>
#include <stdio.h>
#include <time.h>
#include <wchar.h>
]],
          [[return ! wmemmove ((wchar_t *) 0, (const wchar_t *) 0, 0);]])
       ],
       [gl_cv_func_wmemmove=yes],
       [gl_cv_func_wmemmove=no])
    ])
  if test $gl_cv_func_wmemmove = no; then
    HAVE_WMEMMOVE=0
  fi
])
