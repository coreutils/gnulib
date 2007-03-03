# strerror.m4 serial 3
dnl Copyright (C) 2002, 2007 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_STRERROR],
[
  AC_CHECK_FUNCS_ONCE([strerror])
  if test $ac_cv_func_strerror = no; then
    AC_LIBOBJ([strerror])
    gl_PREREQ_STRERROR
  fi
])

# Like gl_FUNC_STRERROR, except prepare for separate compilation (no AC_LIBOBJ).
AC_DEFUN([gl_FUNC_STRERROR_SEPARATE],
[
  AC_CHECK_FUNCS_ONCE([strerror])
  gl_PREREQ_STRERROR
])

# Prerequisites of lib/strerror.c.
AC_DEFUN([gl_PREREQ_STRERROR], [
  :
])
