# fchdir.m4 serial 1
dnl Copyright (C) 2006 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_FCHDIR],
[
  AC_CHECK_FUNCS_ONCE([fchdir])
  if test $ac_cv_func_fchdir = no; then
    AC_LIBOBJ([fchdir])
    gl_PREREQ_FCHDIR
    AC_DEFINE([FCHDIR_REPLACEMENT], 1,
      [Define if gnulib's fchdir() replacement is used.])
    gl_ABSOLUTE_HEADER([dirent.h])
    ABSOLUTE_DIRENT_H=\"$gl_cv_absolute_dirent_h\"
    DIRENT_H='dirent.h'
    UNISTD_H2='unistd.h'
  else
    DIRENT_H=
    UNISTD_H2=
  fi
  AC_SUBST([ABSOLUTE_DIRENT_H])
  AC_SUBST([DIRENT_H])
  AC_SUBST([UNISTD_H2])
])

# Prerequisites of lib/fchdir.c.
AC_DEFUN([gl_PREREQ_FCHDIR], [:])
