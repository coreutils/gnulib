# intlsolaris.m4 serial 2
dnl Copyright (C) 2015-2018 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl
dnl This file can be used in projects which are not available under
dnl the GNU General Public License or the GNU Library General Public
dnl License but which still want to provide support for the GNU gettext
dnl functionality.
dnl Please note that the actual code of the GNU gettext library is covered
dnl by the GNU Library General Public License, and the rest of the GNU
dnl gettext package is covered by the GNU General Public License.
dnl They are *not* in the public domain.

dnl Checks for special localename support needed on Solaris.
dnl Sets gt_nameless_locales.
AC_DEFUN([gt_INTL_SOLARIS],
[
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles

  dnl Persuade Solaris <locale.h> to define 'locale_t'.
  AC_REQUIRE([AC_USE_SYSTEM_EXTENSIONS])

  AC_CHECK_FUNCS_ONCE([uselocale])

  gt_nameless_locales=no
  if test $ac_cv_func_uselocale = yes; then
    AC_CACHE_CHECK([for Solaris 11.4 locale system],
      [gt_cv_locale_solaris114],
      [case "$host_os" in
         solaris*)
           dnl Test whether <locale.h> defines locale_t as a typedef of
           dnl 'struct _LC_locale_t **' (whereas Illumos defines it as a
           dnl typedef of 'struct _locale *').
           AC_COMPILE_IFELSE(
             [AC_LANG_PROGRAM([[
                #include <locale.h>
                struct _LC_locale_t *x;
                locale_t y;
              ]],
              [[*y = x;]])],
             [gt_cv_locale_solaris114=yes],
             [gt_cv_locale_solaris114=no])
           ;;
         *) gt_cv_locale_solaris114=no ;;
       esac
      ])
  fi
  if test $gt_cv_locale_solaris114 = yes; then
    gt_nameless_locales=yes
    AC_DEFINE([HAVE_NAMELESS_LOCALES], [1],
      [Define if the locale_t type does not contain the name of each locale category.])
  fi

  dnl Solaris 12 will maybe provide getlocalename_l.  If it does, it will
  dnl simplify the implementation of gl_locale_name_thread().  But the overrides
  dnl of newlocale, duplocale, freelocale will still be necessary, in order to
  dnl keep the libintl_locale_hash_table up-to-date, which may be used by
  dnl libintl or gnulib code that was compiled on Solaris 11.4, before
  dnl getlocalename_l was introduced.
  if test $ac_cv_func_uselocale = yes; then
    AC_CHECK_FUNCS([getlocalename_l])
  fi
])
