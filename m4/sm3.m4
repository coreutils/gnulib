# sm3.m4 serial 1
dnl Copyright (C) 2017 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_SM3],
[
  dnl Prerequisites of lib/sm3.c.
  AC_REQUIRE([gl_BIGENDIAN])

  dnl Set HAVE_OPENSSL_SM3=0 explicitly
  HAVE_OPENSSL_SM3=0
])
