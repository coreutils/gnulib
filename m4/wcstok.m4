# wcstok.m4 serial 3
dnl Copyright (C) 2011-2019 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_WCSTOK],
[
  AC_REQUIRE([gl_WCHAR_H_DEFAULTS])
  AC_CHECK_FUNCS_ONCE([wcstok])
  if test $ac_cv_func_wcstok = yes; then
    dnl POSIX:        wchar_t *wcstok (wchar_t *, const wchar_t *, wchar_t **);
    dnl mingw, MSVC:  wchar_t *wcstok (wchar_t *, const wchar_t *);
    AC_CACHE_CHECK([for wcstok with POSIX signature],
      [gl_cv_func_wcstok_posix_signature],
      [AC_COMPILE_IFELSE(
         [AC_LANG_PROGRAM(
            [[
/* Tru64 with Desktop Toolkit C has a bug: <stdio.h> must be included before
   <wchar.h>.
   BSD/OS 4.0.1 has a bug: <stddef.h>, <stdio.h> and <time.h> must be
   included before <wchar.h>.  */
#include <stddef.h>
#include <stdio.h>
#include <time.h>
#include <wchar.h>
wchar_t *wcstok (wchar_t *, const wchar_t *, wchar_t **);
]],
            [])],
         [gl_cv_func_wcstok_posix_signature=yes],
         [gl_cv_func_wcstok_posix_signature=no])
      ])
    if test $gl_cv_func_wcstok_posix_signature = no; then
      REPLACE_WCSTOK=1
    fi
  else
    HAVE_WCSTOK=0
  fi
])
