# ptsname_r.m4 serial 1
dnl Copyright (C) 2010-2011 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_PTSNAME_R],
[
  AC_REQUIRE([gl_UNISTD_H_DEFAULTS])

  dnl Persuade glibc <stdlib.h> to declare ptsname_r().
  AC_REQUIRE([AC_USE_SYSTEM_EXTENSIONS])

  AC_CHECK_FUNCS_ONCE([ptsname_r])
  if test $ac_cv_func_ptsname_r = no; then
    HAVE_PTSNAME_R=0
  fi
])

# Prerequisites of lib/ptsname.c.
AC_DEFUN([gl_PREREQ_PTSNAME_R], [
  :
])
