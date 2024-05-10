# stddef_h.m4
# serial 1
dnl Copyright 2024 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl A placeholder for <stdbit.h>, for platforms that have issues.

AC_DEFUN_ONCE([gl_STDBIT_H],
[
  AC_REQUIRE([gl_BIGENDIAN])
  AC_CHECK_HEADERS_ONCE([stdbit.h])
  if test $ac_cv_header_stdbit_h = yes; then
    GL_GENERATE_STDBIT_H=false
  else
    GL_GENERATE_STDBIT_H=true
  fi
])
