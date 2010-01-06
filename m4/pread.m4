# pread.m4 serial 2
dnl Copyright (C) 2009-2010 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_PREAD],
[
  AC_REQUIRE([gl_UNISTD_H_DEFAULTS])
  dnl Persuade glibc <unistd.h> to declare pread().
  AC_REQUIRE([AC_USE_SYSTEM_EXTENSIONS])

  AC_CHECK_FUNCS_ONCE([pread])
  if test $ac_cv_func_pread = no; then
    HAVE_PREAD=0
    AC_LIBOBJ([pread])
  fi
])
