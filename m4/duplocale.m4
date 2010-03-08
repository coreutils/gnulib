# duplocale.m4 serial 2
dnl Copyright (C) 2009, 2010 Free Software Foundation, Inc.
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
    AC_REQUIRE([gl_LOCALE_H])
    AC_CACHE_CHECK([whether duplocale(LC_GLOBAL_LOCALE) works],
      [gl_cv_func_duplocale_works],
      [AC_TRY_RUN([
#include <locale.h>
#if HAVE_XLOCALE_H
# include <xlocale.h>
#endif
int main ()
{
  (void) duplocale (LC_GLOBAL_LOCALE);
  return 0;
}], [gl_cv_func_duplocale_works=yes], [gl_cv_func_duplocale_works=no],
         [dnl Guess it works except on glibc < 2.12.
          AC_EGREP_CPP([Unlucky GNU user], [
#include <features.h>
#ifdef __GNU_LIBRARY__
 #if (__GLIBC__ == 2 && __GLIBC_MINOR__ < 12)
  Unlucky GNU user
 #endif
#endif
            ],
            [gl_cv_func_duplocale_works="guessing no"],
            [gl_cv_func_duplocale_works="guessing yes"])
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
