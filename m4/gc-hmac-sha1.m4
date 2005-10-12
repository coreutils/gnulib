# gc-hmac-sha1.m4 serial 1
dnl Copyright (C) 2005 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_GC_HMAC_SHA1],
[
  AC_REQUIRE([gl_GC])
  AC_DEFINE(GC_USE_HMAC_SHA1, 1,
    [Define to if you want to support HMAC-SHA1 through GC.])
  if test "$ac_cv_libgcrypt" != yes; then
    gl_SHA1
    gl_HMAC_SHA1
    gl_MEMXOR
  fi
])
