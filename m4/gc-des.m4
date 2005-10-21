# gc-des.m4 serial 1
dnl Copyright (C) 2005 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_GC_DES],
[
  AC_REQUIRE([gl_GC])
  AC_DEFINE(GC_USE_DES, 1, [Define if you want to support DES through GC.])
  if test "$ac_cv_libgcrypt" != yes; then
    gl_DES
  fi
])
