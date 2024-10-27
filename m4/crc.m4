# crc.m4
# serial 3
dnl Copyright (C) 2024-2024 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN([gl_CRC_SLICE_BY_8],
[
  AC_MSG_CHECKING([whether to enable CRC slice-by-8 algorithm])
  AC_ARG_ENABLE([crc-slice-by-8],
    [AS_HELP_STRING([[--disable-crc-slice-by-8]],
       [disable CRC slice-by-8 algorithm])],
    [enable_crc_slice_by_8=$enableval],
    [enable_crc_slice_by_8=yes])

  if test $enable_crc_slice_by_8 = yes; then
      AC_DEFINE([GL_CRC_SLICE_BY_8], [1],
      [Define to get faster but larger CRC32 operation.])
  fi
])
