# ptsname.m4 serial 3
dnl Copyright (C) 2010-2015 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_PTSNAME],
[
  AC_REQUIRE([gl_UNISTD_H_DEFAULTS])

  dnl Persuade glibc <stdlib.h> to declare ptsname().
  AC_REQUIRE([AC_USE_SYSTEM_EXTENSIONS])

  AC_CHECK_FUNCS([ptsname])
  if test $ac_cv_func_ptsname = no; then
    HAVE_PTSNAME=0
  else
    AC_CACHE_CHECK([whether ptsname sets errno on failure],
      [gl_cv_func_ptsname_sets_errno],
      [AC_RUN_IFELSE(
         [AC_LANG_PROGRAM([[#include <errno.h>
      ]], [[
      return ptsname (-1) || !errno;
           ]])],
         [gl_cv_func_ptsname_sets_errno=yes],
         [gl_cv_func_ptsname_sets_errno=no],
         [case "$host_os" in
                    # Guess yes on glibc systems.
            *-gnu*) gl_cv_func_ptsname_sets_errno="guessing yes" ;;
                    # If we don't know, assume the worst.
            *)      gl_cv_func_ptsname_sets_errno="guessing no" ;;
          esac
         ])])
    case $gl_cv_func_ptsname_sets_errno in
      *no) REPLACE_PTSNAME=1 ;;
    esac
  fi
])

# Prerequisites of lib/ptsname.c.
AC_DEFUN([gl_PREREQ_PTSNAME], [
  :
])
