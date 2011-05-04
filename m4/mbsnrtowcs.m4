# mbsnrtowcs.m4 serial 3
dnl Copyright (C) 2008, 2010-2011 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_MBSNRTOWCS],
[
  AC_REQUIRE([gl_WCHAR_H_DEFAULTS])

  dnl Persuade glibc <wchar.h> to declare mbsnrtowcs().
  AC_REQUIRE([AC_USE_SYSTEM_EXTENSIONS])

  AC_REQUIRE([AC_TYPE_MBSTATE_T])
  gl_MBSTATE_T_BROKEN

  AC_CHECK_FUNCS_ONCE([mbsnrtowcs])
  if test $ac_cv_func_mbsnrtowcs = no; then
    HAVE_MBSNRTOWCS=0
  else
    if test $REPLACE_MBSTATE_T = 1; then
      REPLACE_MBSNRTOWCS=1
    fi
  fi
  if test $HAVE_MBSNRTOWCS = 0 || test $REPLACE_MBSNRTOWCS = 1; then
    AC_LIBOBJ([mbsnrtowcs])
    AC_LIBOBJ([mbsrtowcs-state])
    gl_PREREQ_MBSNRTOWCS
  fi
])

# Prerequisites of lib/mbsnrtowcs.c.
AC_DEFUN([gl_PREREQ_MBSNRTOWCS], [
  :
])
