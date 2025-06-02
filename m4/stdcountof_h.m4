# stdcountof_h.m4
# serial 1
dnl Copyright 2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN_ONCE([gl_STDCOUNTOF_H],
[
  AC_CHECK_HEADERS_ONCE([stdcountof.h])
  if test $ac_cv_header_stdcountof_h = yes; then
    HAVE_STDCOUNTOF_H=1
  else
    HAVE_STDCOUNTOF_H=0
  fi
  AC_SUBST([HAVE_STDCOUNTOF_H])

  if test $HAVE_STDCOUNTOF_H = 1; then
    GL_GENERATE_STDCOUNTOF_H=false
  else
    GL_GENERATE_STDCOUNTOF_H=true
  fi
])
