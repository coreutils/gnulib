# tzname.m4
# serial 1
dnl Copyright (C) 2024-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

dnl gl_TZNAME tests how the time zone can be obtained.
dnl It is similar AC_STRUCT_TIMEZONE, but also supports MSVC.
dnl It defines
dnl   * the C macro HAVE_STRUCT_TM_TM_ZONE to 1 if 'struct tm' has a field
dnl     'tm_zone',
dnl   * otherwise:
dnl     - the C macro HAVE_TZNAME_ARRAY to 1 if there the rvalue 'tzname'
dnl       or (on native Windows) '_tzname' is usable,
dnl     - the C macro NEED_DECL_TZNAME to 1 if 'tzname' needs to be declared
dnl         extern char *tzname[];
dnl       before use.
AC_DEFUN([gl_TZNAME],
[
  AC_REQUIRE([AC_CANONICAL_HOST])

  dnl Set ac_cv_member_struct_tm_tm_zone,
  dnl     ac_cv_var_tzname, ac_cv_have_decl_tzname.
  dnl Possibly define HAVE_STRUCT_TM_TM_ZONE,
  dnl                 HAVE_TZNAME, HAVE_DECL_TZNAME.
  AC_REQUIRE([AC_STRUCT_TIMEZONE])

  dnl If 'struct tm' has a field 'tm_zone', don't test for tzname or _tzname.
  dnl Rationale: Some code assumes that HAVE_STRUCT_TM_TM_ZONE and HAVE_TZNAME
  dnl are exclusive.
  if test "$ac_cv_member_struct_tm_tm_zone" != yes; then
    if test $ac_cv_var_tzname = yes && test $ac_cv_have_decl_tzname != yes; then
      AC_DEFINE([NEED_DECL_TZNAME], [1],
        [Define to 1 if tzname exists but needs to be declared.])
    fi
    AC_CACHE_CHECK([for tzname array],
      [gl_cv_var_tzname],
      [AC_LINK_IFELSE(
         [AC_LANG_PROGRAM(
            [[#include <time.h>
              #if NEED_DECL_TZNAME
              extern char *tzname[];
              #endif
              #if defined _WIN32 && !defined __CYGWIN__
               #undef tzname
               #define tzname _tzname
              #endif
            ]],
            [[return tzname[0][0];
            ]])
         ],
         [gl_cv_var_tzname=yes],
         [gl_cv_var_tzname=no])
      ])
    if test $gl_cv_var_tzname = yes; then
      AC_DEFINE([HAVE_TZNAME_ARRAY], [1],
                [Define to 1 if 'struct tm' does not have a field 'tm_zone'
                 but instead 'tzname' is usable.])
    fi
  fi
])
