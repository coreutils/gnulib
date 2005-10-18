# md4.m4 serial 1
dnl Copyright (C) 2002, 2003, 2004, 2005 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_MD4],
[
  AC_LIBSOURCES([md4.c, md4.h])
  AC_LIBOBJ([md4])

  dnl Prerequisites of lib/md4.c.
  AC_REQUIRE([AC_C_BIGENDIAN])
])
