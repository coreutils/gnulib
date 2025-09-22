# lchown.m4
# serial 18
dnl Copyright (C) 1998, 2001, 2003-2007, 2009-2025 Free Software Foundation,
dnl Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

# Determine whether we need the lchown wrapper.

dnl From Jim Meyering.
dnl Provide lchown on systems that lack it, and work around bugs
dnl on systems that have it.

AC_DEFUN([gl_FUNC_LCHOWN],
[
  AC_REQUIRE([gl_UNISTD_H_DEFAULTS])
  AC_REQUIRE([gl_FUNC_CHOWN])
  gl_CHECK_FUNCS_ANDROID([lchmod], [[#include <sys/stat.h>]])
  AC_CHECK_FUNCS([lchown])
  if test $ac_cv_func_lchown = no; then
    HAVE_LCHOWN=0
  else
    dnl Trailing slash and ctime bugs in chown also occur in lchown.
    case "$gl_cv_func_chown_slash_works" in
      *yes) ;;
      *)
        REPLACE_LCHOWN=1
        ;;
    esac
    case "$gl_cv_func_chown_ctime_works" in
      *yes) ;;
      *)
        REPLACE_LCHOWN=1
        ;;
    esac
  fi
])
