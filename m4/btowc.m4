# btowc.m4 serial 1
dnl Copyright (C) 2008 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_BTOWC],
[
  AC_REQUIRE([gl_WCHAR_H_DEFAULTS])

  AC_CHECK_FUNCS_ONCE([btowc])
  if test $ac_cv_func_btowc = no; then
    HAVE_BTOWC=0
    AC_LIBOBJ([btowc])
    gl_PREREQ_BTOWC
  fi
])

# Prerequisites of lib/btowc.c.
AC_DEFUN([gl_PREREQ_BTOWC], [
  :
])
