# newlocale.m4
# serial 4
dnl Copyright (C) 2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN([gl_FUNC_NEWLOCALE],
[
  AC_REQUIRE([gl_LOCALE_H_DEFAULTS])
  AC_REQUIRE([gl_LOCALE_T])
  if test $HAVE_LOCALE_T = 1; then
    gl_CHECK_FUNCS_ANDROID([newlocale], [[#include <locale.h>]])
    gl_func_newlocale="$ac_cv_func_newlocale"
  else
    dnl In 2019, some versions of z/OS lack the locale_t type and have broken
    dnl newlocale, duplocale, freelocale functions.
    gl_cv_onwards_func_newlocale='future OS version'
    gl_func_newlocale=no
  fi
  if test $gl_func_newlocale = yes; then
    dnl Check against the macOS, NetBSD, Solaris 11 OpenIndiana bug:
    dnl When the third argument is NULL, newlocale() uses locale category data
    dnl from the current locale instead of from the "C" locale.
    gl_CHECK_FUNCS_ANDROID([nl_langinfo_l], [[#include <langinfo.h>]])
    if test $ac_cv_func_nl_langinfo_l = yes; then
      AC_REQUIRE([AC_CANONICAL_HOST])
      AC_CACHE_CHECK([whether newlocale with a null base works],
        [gl_cv_func_newlocale_works],
        [dnl Prepare a guess, used when cross-compiling or when specific locales
         dnl are not available.
         case "$host_os" in
           darwin* | netbsd* | solaris*)
             gl_cv_func_newlocale_works="guessing no" ;;
           *)
             gl_cv_func_newlocale_works="guessing yes" ;;
         esac
         AC_RUN_IFELSE(
           [AC_LANG_SOURCE([[
#include <locale.h>
#if HAVE_XLOCALE_H
# include <xlocale.h>
#endif
#include <langinfo.h>
int main ()
{
  locale_t l1 = newlocale (LC_TIME_MASK, "en_US.UTF-8", NULL);
  if (l1 != NULL)
    {
      if (setlocale (LC_ALL, "fr_FR.UTF-8") != NULL)
        {
          locale_t l1a = newlocale (LC_TIME_MASK, "en_US.UTF-8", NULL);
          const char *radixchar1a = nl_langinfo_l (RADIXCHAR, l1a);
          return (*radixchar1a != '.');
        }
    }
  return 2;
}]])],
           [gl_cv_func_newlocale_works=yes],
           [if test $? = 1; then
              gl_cv_func_newlocale_works=no
            fi
           ],
           [:])
        ])
      case "$gl_cv_func_newlocale_works" in
        *yes) ;;
        *) REPLACE_NEWLOCALE=1 ;;
      esac
    fi
  else
    HAVE_NEWLOCALE=0
    case "$gl_cv_onwards_func_newlocale" in
      future*) REPLACE_NEWLOCALE=1 ;;
    esac
  fi
])

# Prerequisites of lib/newlocale.c.
AC_DEFUN([gl_PREREQ_NEWLOCALE],
[
  :
])
