# intl-thread-locale.m4 serial 1
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

dnl Check how to retrieve the name of a per-thread locale (POSIX locale_t).
dnl Sets gt_nameless_locales.
AC_DEFUN([gt_INTL_THREAD_LOCALE_NAME],
[
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles

  dnl Persuade Solaris <locale.h> to define 'locale_t'.
  AC_REQUIRE([AC_USE_SYSTEM_EXTENSIONS])

  AC_CHECK_FUNCS_ONCE([uselocale])

  if test $ac_cv_func_uselocale = yes; then
    AC_CACHE_CHECK([for Solaris 11.4 locale system],
      [gt_cv_locale_solaris114],
      [case "$host_os" in
         solaris*)
           dnl Test whether <locale.h> defines locale_t as a typedef of
           dnl 'struct _LC_locale_t **' (whereas Illumos defines it as a
           dnl typedef of 'struct _locale *').
           dnl Another possible test would be to include <sys/localedef.h>
           dnl and test whether it defines the _LC_core_data_locale_t type.
           dnl This type was added in Solaris 11.4.
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
    AC_DEFINE([HAVE_SOLARIS114_LOCALES], [1],
      [Define if the locale_t type is as on Solaris 11.4.])
  fi

  dnl Solaris 12 will maybe provide getlocalename_l.  If it does, it will
  dnl improve the implementation of gl_locale_name_thread(), by removing
  dnl the use of undocumented structures.
  if test $ac_cv_func_uselocale = yes; then
    AC_CHECK_FUNCS([getlocalename_l])
  fi

  dnl This code is for future use, in case we some day have to port to a
  dnl platform where the locale_t type does not provide access to the name of
  dnl each locale category.  This code has the drawback that it requires the
  dnl gnulib overrides of 'newlocale', 'duplocale', 'freelocale', which is a
  dnl problem for GNU libunistring.  Therefore try hard to avoid enabling this
  dnl code!
  gt_nameless_locales=no
  if false; then
    gt_nameless_locales=yes
    AC_DEFINE([HAVE_NAMELESS_LOCALES], [1],
      [Define if the locale_t type does not contain the name of each locale category.])
  fi
])
