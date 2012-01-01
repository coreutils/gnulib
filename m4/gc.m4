# gc.m4 serial 8
dnl Copyright (C) 2005-2006, 2009-2012 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_GC],
[
  AC_ARG_WITH([libgcrypt],
    AS_HELP_STRING([--with-libgcrypt], [use libgcrypt for low-level crypto]),
    libgcrypt=$withval, libgcrypt=no)
  if test "$libgcrypt" != no; then
    AC_LIB_HAVE_LINKFLAGS([gcrypt], [gpg-error], [#include <gcrypt.h>])
  fi
])
