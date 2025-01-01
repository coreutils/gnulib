# sethostname.m4
# serial 3
dnl Copyright (C) 2011-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

# Ensure
# - the sethostname() function,
# - the HOST_NAME_MAX macro in <limits.h>.
AC_DEFUN([gl_FUNC_SETHOSTNAME],
[
  AC_REQUIRE([gl_UNISTD_H_DEFAULTS])

  gl_PREREQ_HOST_NAME_MAX

  gl_CHECK_FUNCS_ANDROID([sethostname], [[#include <unistd.h>]])
  if test $ac_cv_func_sethostname = no; then
    HAVE_SETHOSTNAME=0
    case "$gl_cv_onwards_func_sethostname" in
      future*) REPLACE_SETHOSTNAME=1 ;;
    esac
  fi

  AC_CHECK_DECLS([sethostname])
  if test $ac_cv_have_decl_sethostname = no; then
    HAVE_DECL_SETHOSTNAME=0
  fi
])
