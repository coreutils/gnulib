# rijndael.m4 serial 1
dnl Copyright (C) 2005 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_RIJNDAEL],
[
  AC_LIBSOURCES([rijndael-alg-fst.c, rijndael-alg-fst.h])
  AC_LIBOBJ([rijndael-alg-fst])
  AC_LIBSOURCES([rijndael-api-fst.c, rijndael-api-fst.h])
  AC_LIBOBJ([rijndael-api-fst])
])
