# wcwidth.m4 serial 10
dnl Copyright (C) 2006, 2007 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_WCWIDTH],
[
  AC_REQUIRE([gl_WCHAR_H_DEFAULTS])

  dnl Persuade glibc <wchar.h> to declare wcwidth().
  AC_REQUIRE([AC_GNU_SOURCE])

  AC_REQUIRE([gt_TYPE_WCHAR_T])
  AC_REQUIRE([gt_TYPE_WINT_T])

  AC_CHECK_HEADERS_ONCE([wchar.h])
  AC_CHECK_FUNCS_ONCE([wcwidth])

  AC_CHECK_DECLS([wcwidth], [], [], [
/* AIX 3.2.5 declares wcwidth in <string.h>. */
#include <string.h>
/* Tru64 with Desktop Toolkit C has a bug: <stdio.h> must be included before
   <wchar.h>.
   BSD/OS 4.0.1 has a bug: <stddef.h>, <stdio.h> and <time.h> must be included
   before <wchar.h>.  */
#include <stddef.h>
#include <stdio.h>
#include <time.h>
#include <wchar.h>
])
  if test $ac_cv_have_decl_wcwidth != yes; then
    HAVE_DECL_WCWIDTH=0
  fi

  if test $ac_cv_func_wcwidth = no; then
    REPLACE_WCWIDTH=1
  fi
  if test $REPLACE_WCWIDTH = 1; then
    AC_LIBOBJ([wcwidth])
  fi

  if test $REPLACE_WCWIDTH = 1 || test $HAVE_DECL_WCWIDTH = 0; then
    WCHAR_H=wchar.h
  fi
])
