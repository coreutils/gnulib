# select.m4 serial 1
dnl Copyright (C) 2009 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_SELECT],
[
  AC_REQUIRE([gl_HEADER_SYS_SELECT])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  if test "$ac_cv_header_winsock2_h" = yes; then
    AC_LIBOBJ([select])
  else
    dnl On Interix 3.5, select(0, NULL, NULL, NULL, timeout) fails with error
    dnl EFAULT.
    AC_CHECK_HEADERS_ONCE([sys/select.h])
    AC_CACHE_CHECK([whether select supports a 0 argument],
      [gl_cv_func_select_supports0],
      [
        AC_TRY_RUN([
#include <sys/types.h>
#include <sys/time.h>
#if HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif
int main ()
{
  struct timeval timeout;
  timeout.tv_sec = 0;
  timeout.tv_usec = 5;
  return select (0, (fd_set *)0, (fd_set *)0, (fd_set *)0, &timeout) < 0;
}], [gl_cv_func_select_supports0=yes], [gl_cv_func_select_supports0=no],
          [
changequote(,)dnl
           case "$host_os" in
                       # Guess no on Interix.
             interix*) gl_cv_func_select_supports0="guessing no";;
                       # Guess yes otherwise.
             *)        gl_cv_func_select_supports0="guessing yes";;
           esac
changequote([,])dnl
          ])
      ])
    case "$gl_cv_func_select_supports0" in
      *yes) ;;
      *)
        REPLACE_SELECT=1
        AC_LIBOBJ([select])
        ;;
    esac
  fi
])
