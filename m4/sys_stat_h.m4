# sys_stat_h.m4 serial 14   -*- Autoconf -*-
dnl Copyright (C) 2006-2009 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl From Eric Blake.
dnl Provide a GNU-like <sys/stat.h>.

AC_DEFUN([gl_HEADER_SYS_STAT_H],
[
  AC_REQUIRE([gl_SYS_STAT_H_DEFAULTS])

  dnl Check for lstat.  Systems that lack it (mingw) also lack symlinks, so
  dnl stat is a good replacement.
  AC_CHECK_FUNCS_ONCE([lstat])
  if test $ac_cv_func_lstat = yes; then
    HAVE_LSTAT=1
  else
    HAVE_LSTAT=0
  fi
  AC_SUBST([HAVE_LSTAT])

  dnl For the mkdir substitute.
  AC_REQUIRE([AC_C_INLINE])

  dnl Check for broken stat macros.
  AC_REQUIRE([AC_HEADER_STAT])

  gl_CHECK_NEXT_HEADERS([sys/stat.h])

  dnl Define types that are supposed to be defined in <sys/types.h> or
  dnl <sys/stat.h>.
  AC_CHECK_TYPE([nlink_t], [],
    [AC_DEFINE([nlink_t], [int],
       [Define to the type of st_nlink in struct stat, or a supertype.])],
    [#include <sys/types.h>
     #include <sys/stat.h>])

]) # gl_HEADER_SYS_STAT_H

AC_DEFUN([gl_SYS_STAT_MODULE_INDICATOR],
[
  dnl Use AC_REQUIRE here, so that the default settings are expanded once only.
  AC_REQUIRE([gl_SYS_STAT_H_DEFAULTS])
  GNULIB_[]m4_translit([$1],[abcdefghijklmnopqrstuvwxyz./-],[ABCDEFGHIJKLMNOPQRSTUVWXYZ___])=1
])

AC_DEFUN([gl_SYS_STAT_H_DEFAULTS],
[
  AC_REQUIRE([gl_UNISTD_H_DEFAULTS]) dnl for REPLACE_FCHDIR
  GNULIB_FCHMODAT=0;  AC_SUBST([GNULIB_FCHMODAT])
  GNULIB_FSTATAT=0;   AC_SUBST([GNULIB_FSTATAT])
  GNULIB_LCHMOD=0;    AC_SUBST([GNULIB_LCHMOD])
  GNULIB_LSTAT=0;     AC_SUBST([GNULIB_LSTAT])
  GNULIB_MKDIRAT=0;   AC_SUBST([GNULIB_MKDIRAT])
  GNULIB_MKFIFOAT=0;  AC_SUBST([GNULIB_MKFIFOAT])
  GNULIB_MKNODAT=0;   AC_SUBST([GNULIB_MKNODAT])
  dnl Assume proper GNU behavior unless another module says otherwise.
  HAVE_FCHMODAT=1;    AC_SUBST([HAVE_FCHMODAT])
  HAVE_FSTATAT=1;     AC_SUBST([HAVE_FSTATAT])
  HAVE_LCHMOD=1;      AC_SUBST([HAVE_LCHMOD])
  HAVE_MKDIRAT=1;     AC_SUBST([HAVE_MKDIRAT])
  HAVE_MKFIFOAT=1;    AC_SUBST([HAVE_MKFIFOAT])
  HAVE_MKNODAT=1;     AC_SUBST([HAVE_MKNODAT])
  REPLACE_FSTATAT=0;  AC_SUBST([REPLACE_FSTATAT])
  REPLACE_LSTAT=0;    AC_SUBST([REPLACE_LSTAT])
  REPLACE_MKDIR=0;    AC_SUBST([REPLACE_MKDIR])
])
