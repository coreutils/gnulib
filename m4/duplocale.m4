# duplocale.m4 serial 5
dnl Copyright (C) 2009-2011 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_DUPLOCALE],
[
  AC_REQUIRE([gl_LOCALE_H_DEFAULTS])
  AC_REQUIRE([AC_CANONICAL_HOST])
  AC_CHECK_FUNCS_ONCE([duplocale])
  if test $ac_cv_func_duplocale = yes; then
    dnl Check against glibc bug where duplocale crashes.
    dnl See <http://sourceware.org/bugzilla/show_bug.cgi?id=10969>.
    dnl Also, on AIX 7.1, duplocale(LC_GLOBAL_LOCALE) returns (locale_t)0 with
    dnl errno set to EINVAL.
    AC_REQUIRE([gl_LOCALE_H])
    AC_CACHE_CHECK([whether duplocale(LC_GLOBAL_LOCALE) works],
      [gl_cv_func_duplocale_works],
      [AC_RUN_IFELSE(
         [AC_LANG_SOURCE([[
#include <locale.h>
#if HAVE_XLOCALE_H
# include <xlocale.h>
#endif
int main ()
{
  if (duplocale (LC_GLOBAL_LOCALE) == (locale_t)0)
    return 1;
  return 0;
}]])],
         [gl_cv_func_duplocale_works=yes],
         [gl_cv_func_duplocale_works=no],
         [dnl Guess it works except on glibc < 2.12, uClibc, and AIX.
          case "$host_os" in
            aix*) gl_cv_func_duplocale_works="guessing no";;
            *-gnu*)
              AC_EGREP_CPP([Unlucky], [
#include <features.h>
#ifdef __GNU_LIBRARY__
 #if (__GLIBC__ == 2 && __GLIBC_MINOR__ < 12)
  Unlucky GNU user
 #endif
#endif
#ifdef __UCLIBC__
 Unlucky user
#endif
                ],
                [gl_cv_func_duplocale_works="guessing no"],
                [gl_cv_func_duplocale_works="guessing yes"])
              ;;
            *) gl_cv_func_duplocale_works="guessing yes";;
          esac
         ])
      ])
    case "$gl_cv_func_duplocale_works" in
      *no) REPLACE_DUPLOCALE=1 ;;
    esac
  else
    HAVE_DUPLOCALE=0
  fi
  if test $REPLACE_DUPLOCALE = 1; then
    gl_REPLACE_LOCALE_H
    AC_LIBOBJ([duplocale])
    gl_PREREQ_DUPLOCALE
  fi
])

# Prerequisites of lib/duplocale.c.
AC_DEFUN([gl_PREREQ_DUPLOCALE],
[
  :
])
