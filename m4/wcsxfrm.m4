# wcsxfrm.m4
# serial 2
dnl Copyright (C) 2011-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN([gl_FUNC_WCSXFRM],
[
  AC_REQUIRE([gl_WCHAR_H_DEFAULTS])
  AC_CHECK_FUNCS_ONCE([wcsxfrm])
  if test $ac_cv_func_wcsxfrm = no; then
    HAVE_WCSXFRM=0
  fi
])
