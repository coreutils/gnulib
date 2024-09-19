# spawn-pipe.m4 serial 3
dnl Copyright (C) 2004, 2008-2024 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_SPAWN_PIPE],
[
  dnl Prerequisites of lib/spawn-pipe.c.
  AC_REQUIRE([AC_TYPE_MODE_T])

  AC_CHECK_HEADERS_ONCE([libcx/spawn2.h])
  if test $ac_cv_header_libcx_spawn2_h = yes; then
    HAVE_LIBCX_SPAWN2_H=1
  else
    HAVE_LIBCX_SPAWN2_H=0
  fi
  AC_SUBST(HAVE_LIBCX_SPAWN2_H)
])
