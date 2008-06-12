# open.m4 serial 2
dnl Copyright (C) 2007-2008 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_OPEN],
[
  AC_REQUIRE([gl_FCNTL_H_DEFAULTS])
  AC_REQUIRE([AC_CANONICAL_HOST])
  case "$host_os" in
    mingw* | pw*)
      REPLACE_OPEN=1
      AC_LIBOBJ([open])
      ;;
    *)
      dnl open("foo/") should not create a file when the file name has a
      dnl trailing slash.
      AC_CACHE_CHECK([whether open recognizes a trailing slash],
        [gl_cv_func_open_slash],
        [
          AC_TRY_RUN([
#include <fcntl.h>
#if HAVE_UNISTD_H
# include <unistd.h>
#endif
int main ()
{
  return open ("conftest.sl/", O_CREAT, 0600) >= 0;
}], [gl_cv_func_open_slash=yes], [gl_cv_func_open_slash=no],
            [
changequote(,)dnl
             case "$host_os" in
               solaris2.[0-9]*) gl_cv_func_open_slash="guessing no" ;;
               hpux*)           gl_cv_func_open_slash="guessing no" ;;
               *)               gl_cv_func_open_slash="guessing yes" ;;
             esac
changequote([,])dnl
            ])
          rm -f conftest.sl
        ])
      case "$gl_cv_func_open_slash" in
        *no)
          AC_DEFINE([OPEN_TRAILING_SLASH_BUG], 1,
            [Define to 1 if open() fails to recognize a trailing slash.])
          REPLACE_OPEN=1
          AC_LIBOBJ([open])
          ;;
      esac
      ;;
  esac
])
