# strfmon_l.m4 serial 1
dnl Copyright (C) 2017-2018 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_STRFMON_L],
[
  AC_REQUIRE([gl_MONETARY_H_DEFAULTS])

  dnl Persuade glibc <monetary.h> to declare strfmon_l().
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])
  dnl On Mac OS X 10.12, <monetary.h> may declare strfmon_l() if
  dnl _USE_EXTENDED_LOCALES_ is defined. But this symbol is supposed
  dnl to be defined by <xlocale.h>, not by us.

  AC_CHECK_FUNCS_ONCE([strfmon_l])
  if test $ac_cv_func_strfmon_l = yes; then
    dnl Test for bug <https://sourceware.org/bugzilla/show_bug.cgi?id=19633>
    dnl which was fixed in glibc-2.24.
    AC_CACHE_CHECK([whether strfmon_l works],
      [gl_cv_strfmon_l_works],
      [AC_EGREP_CPP([Unlucky],
         [
#include <features.h>
#ifdef __GNU_LIBRARY__
 #if (__GLIBC__ == 2 && __GLIBC_MINOR__ < 24)
  Unlucky GNU user
 #endif
#endif
         ],
         [gl_cv_strfmon_l_works=no],
         [gl_cv_strfmon_l_works="guessing yes"])
      ])
    if test "$gl_cv_strfmon_l_works" = no; then
      REPLACE_STRFMON_L=1
    fi
  else
    HAVE_STRFMON_L=0
  fi
])

# Prerequisites of lib/strfmon_l.c.
AC_DEFUN([gl_PREREQ_STRFMON_L], [
  :
])
