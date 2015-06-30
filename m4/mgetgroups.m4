#serial 7
dnl Copyright (C) 2007-2015 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_MGETGROUPS],
[
  AC_CHECK_HEADERS_ONCE([grp.h])
  AC_CHECK_FUNCS_ONCE([getgrouplist])
  if test "$ac_cv_func_getgrouplist" = yes; then
    AC_CACHE_CHECK([for getgrouplist with int signature],
      [gl_cv_getgrouplist_with_int],
      [gl_cv_getgrouplist_with_int=no
       gl_save_c_werror_flag=$ac_c_werror_flag
       ac_c_werror_flag=yes
       AC_COMPILE_IFELSE(
         [AC_LANG_PROGRAM(
            [[#if HAVE_GRP_H
               #include <grp.h>
              #endif
              int groups[1];
              int ngroups = 1;
            ]],
            [[return - getgrouplist ("root", 0, groups, &ngroups);]])],
         [],
         [AC_COMPILE_IFELSE(
           [AC_LANG_PROGRAM(
              [[#if HAVE_GRP_H
                 #include <grp.h>
                #endif
                int groups[sizeof (gid_t) == sizeof (int) ? 1 : -1];
                int ngroups = 1;
              ]],
              [[return - getgrouplist ("root", 0, groups, &ngroups);]])],
            [gl_cv_getgrouplist_with_int=yes])])
       ac_c_werror_flag=$gl_save_c_werror_flag])
    if test "$gl_cv_getgrouplist_with_int" = yes; then
      AC_DEFINE([HAVE_GETGROUPLIST_WITH_INT], 1,
        [Define to 1 if getgrouplist accepts and returns int and not gid_t.])
    fi
  fi
])
