# getusershell.m4
# serial 8
dnl Copyright (C) 2002-2003, 2006, 2008-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN([gl_FUNC_GETUSERSHELL],
[
  AC_REQUIRE([gl_UNISTD_H_DEFAULTS])
  AC_REQUIRE([AC_CANONICAL_HOST])

  dnl Persuade glibc <unistd.h> to declare {get,set,end}usershell().
  AC_REQUIRE([AC_USE_SYSTEM_EXTENSIONS])

  dnl Check whether the getusershell function exists.
  AC_CHECK_FUNCS_ONCE([getusershell])
  if test $ac_cv_func_getusershell = yes; then
    HAVE_GETUSERSHELL=1
    dnl Check whether getusershell is declared.
    AC_CHECK_DECLS([getusershell])
    if test $ac_cv_have_decl_getusershell = no; then
      HAVE_DECL_GETUSERSHELL=0
    fi
  else
    HAVE_GETUSERSHELL=0
    dnl Assume that on platforms which declare it, the function exists.
    HAVE_DECL_GETUSERSHELL=0
  fi
  dnl Replace the function on musl unconditionally.
  dnl See: https://www.openwall.com/lists/musl/2024/05/18/1
  case "$host_os" in
    *-musl* | midipix*) REPLACE_GETUSERSHELL=1 ;;
  esac
])

# Prerequisites of lib/getusershell.c.
AC_DEFUN([gl_PREREQ_GETUSERSHELL], [:])
