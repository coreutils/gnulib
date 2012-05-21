# gc.m4 serial 9
dnl Copyright (C) 2005-2012 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_GC],
[
  AC_ARG_WITH([libgcrypt],
    AS_HELP_STRING([--with-libgcrypt], [use libgcrypt for low-level crypto]),
    libgcrypt=$withval, libgcrypt=no)
  if test "$libgcrypt" != no; then
    AC_LIB_HAVE_LINKFLAGS([gcrypt], [gpg-error], [
#include <gcrypt.h>
/* GCRY_MODULE_ID_USER was added in 1.4.4 and gc-libgcrypt.c
   will fail on startup if we don't have 1.4.4 or later, so
   test for it early. */
#if !defined GCRY_MODULE_ID_USER
error too old libgcrypt
#endif
])
  fi
])
