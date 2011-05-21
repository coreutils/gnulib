# serial 3
# See if we need to provide mkfifo replacement.

dnl Copyright (C) 2009-2011 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

# Written by Eric Blake.

AC_DEFUN([gl_FUNC_MKFIFO],
[
  AC_REQUIRE([gl_SYS_STAT_H_DEFAULTS])
  AC_CHECK_FUNCS_ONCE([mkfifo])
  if test $ac_cv_func_mkfifo = no; then
    HAVE_MKFIFO=0
  else
    dnl Check for Solaris 9 and FreeBSD bug with trailing slash.
    AC_CHECK_FUNCS_ONCE([lstat])
    AC_CACHE_CHECK([whether mkfifo rejects trailing slashes],
      [gl_cv_func_mkfifo_works],
      [# Assume that if we have lstat, we can also check symlinks.
       if test $ac_cv_func_lstat = yes; then
         ln -s conftest.tmp conftest.lnk
       fi
       AC_RUN_IFELSE(
         [AC_LANG_PROGRAM(
           [[#include <sys/stat.h>
           ]],
           [[int result = 0;
             if (!mkfifo ("conftest.tmp/", 0600))
               result |= 1;
#if HAVE_LSTAT
             if (!mkfifo ("conftest.lnk/", 0600))
               result |= 2;
#endif
             return result;
           ]])],
         [gl_cv_func_mkfifo_works=yes], [gl_cv_func_mkfifo_works=no],
         [gl_cv_func_mkfifo_works="guessing no"])
       rm -f conftest.tmp conftest.lnk])
    if test "$gl_cv_func_mkfifo_works" != yes; then
      AC_DEFINE([MKFIFO_TRAILING_SLASH_BUG], [1], [Define to 1 if mkfifo
        does not reject trailing slash])
      REPLACE_MKFIFO=1
    fi
  fi
])
