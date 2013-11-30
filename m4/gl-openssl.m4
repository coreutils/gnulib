# gl-openssl.m4 serial 1
dnl Copyright (C) 2013 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_CRYPTO_CHECK],
[
  AC_ARG_WITH([openssl],
    [AS_HELP_STRING([--with-openssl],
      [use libcrypto hash routines if available: default=no])],
    [],
    [with_openssl=no])

  if test "x$1" = xMD5; then
    ALG_header=md5.h
  else
    ALG_header=sha.h
  fi

  LIB_CRYPTO=
  AC_SUBST([LIB_CRYPTO])
  if test "x$with_openssl" != xno; then
    AC_CHECK_LIB([crypto], [$1],
      [AC_CHECK_HEADERS([openssl/$ALG_header],
        [LIB_CRYPTO='-lcrypto'
         AC_DEFINE([HAVE_OPENSSL_$1],[1],
           [Define to 1 if libcrypto is used for $1])])])
    if test "x$with_openssl" = xyes; then
      if test "x$LIB_CRYPTO" = x; then
        AC_MSG_WARN([openssl development library not found for $1])
      fi
    fi
  fi
])
