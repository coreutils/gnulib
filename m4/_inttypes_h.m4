# _inttypes_h.m4 serial 1
dnl Copyright (C) 2006 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl From Derek Price.

AC_DEFUN([gl_INTTYPES_H],
[AC_LIBSOURCES([inttypes.h])
gl_FULL_HEADER_PATH([inttypes.h])
AC_CHECK_DECLS_ONCE([strtoimax strtoumax])dnl
])
