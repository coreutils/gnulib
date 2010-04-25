# ttyname_r.m4 serial 3
dnl Copyright (C) 2010 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_TTYNAME_R],
[
  AC_REQUIRE([gl_UNISTD_H_DEFAULTS])

  dnl Persuade Solaris <unistd.h> to provide the POSIX compliant declaration of
  dnl ttyname_r().
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])

  AC_CHECK_FUNCS([ttyname_r])
  if test $ac_cv_func_ttyname_r = no; then
    HAVE_TTYNAME_R=0
  else
    dnl On MacOS X 10.4 (and Solaris 10 without gl_USE_SYSTEM_EXTENSIONS)
    dnl the return type is 'char *', not 'int'.
    AC_CACHE_CHECK([whether ttyname_r is compatible with its POSIX signature],
      [gl_cv_func_ttyname_r_posix],
      [AC_COMPILE_IFELSE(
         [AC_LANG_PROGRAM(
            [[#include <stddef.h>
              #include <unistd.h>]],
            [[*ttyname_r (0, NULL, 0);]])
         ],
         [gl_cv_func_ttyname_r_posix=no],
         [gl_cv_func_ttyname_r_posix=yes])
      ])
    if test $gl_cv_func_ttyname_r_posix = no; then
      REPLACE_TTYNAME_R=1
    else
      AC_DEFINE([HAVE_POSIXDECL_TTYNAME_R], [1],
        [Define if the ttyname_r function has a POSIX compliant declaration.])
      dnl On Solaris 10, both ttyname_r functions (the one with the non-POSIX
      dnl declaration and the one with the POSIX declaration) refuse to do
      dnl anything when the output buffer is less than 128 bytes large.
      AC_REQUIRE([AC_CANONICAL_HOST])
      AC_CACHE_CHECK([whether ttyname_r works with small buffers],
        [gl_cv_func_ttyname_r_works],
        [
          dnl Initial guess, used when cross-compiling or when /dev/tty cannot
          dnl be opened.
changequote(,)dnl
          case "$host_os" in
                      # Guess no on Solaris.
            solaris*) gl_cv_func_ttyname_r_works="guessing no" ;;
                      # Guess yes otherwise.
            *)        gl_cv_func_ttyname_r_works="guessing yes" ;;
          esac
changequote([,])dnl
          AC_TRY_RUN([
#include <fcntl.h>
#include <unistd.h>
int
main (void)
{
  int fd;
  char buf[31]; /* use any size < 128 here */

  fd = open ("/dev/tty", O_RDONLY);
  if (fd < 0)
    return 1;
  if (ttyname_r (fd, buf, sizeof (buf)) != 0)
    return 1;
  return 0;
}], [gl_cv_func_ttyname_r_works=yes], [:], [:])
        ])
      case "$gl_cv_func_ttyname_r_works" in
        *yes) ;;
        *) REPLACE_TTYNAME_R=1 ;;
      esac
    fi
  fi
  if test $HAVE_TTYNAME_R = 0 || test $REPLACE_TTYNAME_R = 1; then
    AC_LIBOBJ([ttyname_r])
    gl_PREREQ_TTYNAME_R
  fi
])

# Prerequisites of lib/ttyname_r.c.
AC_DEFUN([gl_PREREQ_TTYNAME_R], [
  AC_CHECK_FUNCS([ttyname])
])
