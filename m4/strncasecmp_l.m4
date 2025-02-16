# strncasecmp_l.m4
# serial 1
dnl Copyright (C) 2009-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN([gl_FUNC_STRNCASECMP_L],
[
  AC_REQUIRE([gl_STRINGS_H_DEFAULTS])

  dnl Persuade glibc <strings.h> to declare strncasecmp_l().
  AC_REQUIRE([AC_USE_SYSTEM_EXTENSIONS])

  AC_CHECK_FUNCS_ONCE([strncasecmp_l])
  if test $ac_cv_func_strncasecmp_l = yes; then
    dnl strncasecmp_l usually has the same bug as strncasecmp.
    gl_STRCASECMP_WORKS
    case "$gl_cv_func_strcasecmp_works" in
      *yes) ;;
      *) REPLACE_STRNCASECMP_L=1 ;;
    esac
  else
    HAVE_STRNCASECMP_L=0
  fi
])

# Prerequisites of lib/strncasecmp_l.c.
AC_DEFUN([gl_PREREQ_STRNCASECMP_L],
[
  AC_REQUIRE([gt_FUNC_USELOCALE])
  :
])
