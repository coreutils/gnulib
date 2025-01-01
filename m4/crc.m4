# crc.m4
# serial 4
dnl Copyright (C) 2024-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

# For use by maintainer's after gl_EARLY in configure.ac.
AC_DEFUN([gl_DISABLE_CRC_SLICE_BY_8],
[
  gl_crc_slice_by_8=no
])

AC_DEFUN([gl_CRC_SLICE_BY_8],
[
  AC_MSG_CHECKING([whether to enable CRC slice-by-8 algorithm])
  if test x"$gl_crc_slice_by_8" != x"no"; then
      AC_MSG_RESULT([yes])
      AC_DEFINE([GL_CRC_SLICE_BY_8], [1],
                [Define to get faster but larger CRC32 operation.])
  else
      AC_MSG_RESULT([no])
  fi
])
