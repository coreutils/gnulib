# canonicalize-lgpl.m4 serial 7
dnl Copyright (C) 2003, 2006-2007, 2009 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_CANONICALIZE_LGPL],
[
  dnl Do this replacement check manually because the file name is shorter
  dnl than the function name.
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])
  AC_CHECK_FUNCS_ONCE([canonicalize_file_name])
  dnl Assume that all platforms with canonicalize_file_name also have
  dnl a working realpath; otherwise assume realpath is broken.
  if test $ac_cv_func_canonicalize_file_name = no; then
    HAVE_CANONICALIZE_FILE_NAME=0
    AC_LIBOBJ([canonicalize-lgpl])
    REPLACE_REALPATH=1
    gl_PREREQ_CANONICALIZE_LGPL
  fi
])

# Like gl_CANONICALIZE_LGPL, except prepare for separate compilation
# (no AC_LIBOBJ).
AC_DEFUN([gl_CANONICALIZE_LGPL_SEPARATE],
[
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])
  AC_CHECK_FUNCS_ONCE([canonicalize_file_name])
  gl_PREREQ_CANONICALIZE_LGPL
])

# Prerequisites of lib/canonicalize-lgpl.c.
AC_DEFUN([gl_PREREQ_CANONICALIZE_LGPL],
[
  AC_CHECK_HEADERS_ONCE([sys/param.h])
  AC_CHECK_FUNCS_ONCE([getcwd readlink])
])
