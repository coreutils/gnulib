# sys_stat_h.m4 serial 7   -*- Autoconf -*-
dnl Copyright (C) 2006-2008 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl From Eric Blake.
dnl Test whether <sys/stat.h> contains lstat and mkdir or must be substituted.

AC_DEFUN([gl_HEADER_SYS_STAT_H],
[
  dnl Check for lstat.  Systems that lack it (mingw) also lack symlinks, so
  dnl stat is a good replacement.
  AC_CHECK_FUNCS_ONCE([lstat])
  if test $ac_cv_func_lstat = yes; then
    HAVE_LSTAT=1
  else
    HAVE_LSTAT=0
  fi
  AC_SUBST([HAVE_LSTAT])

  dnl Check for mkdir.  Mingw has _mkdir(name) in the nonstandard <io.h>
  dnl instead.
  AC_CHECK_DECLS([mkdir],
    [],
    [AC_CHECK_HEADERS([io.h])],
    [#include <sys/stat.h>])
  if test $ac_cv_have_decl_mkdir = yes; then
    HAVE_DECL_MKDIR=1
  else
    HAVE_DECL_MKDIR=0
  fi
  AC_SUBST([HAVE_DECL_MKDIR])
  if test "$ac_cv_header_io_h" = yes; then
    HAVE_IO_H=1
  else
    HAVE_IO_H=0
  fi
  AC_SUBST([HAVE_IO_H])
  AC_REQUIRE([AC_C_INLINE])

  dnl Check for broken stat macros.
  AC_REQUIRE([AC_HEADER_STAT])

  gl_CHECK_NEXT_HEADERS([sys/stat.h])
  SYS_STAT_H='sys/stat.h'
  AC_SUBST([SYS_STAT_H])

  dnl Define types that are supposed to be defined in <sys/types.h> or
  dnl <sys/stat.h>.
  AC_CHECK_TYPE([nlink_t], [],
    [AC_DEFINE([nlink_t], [int],
       [Define to the type of st_nlink in struct stat, or a supertype.])],
    [#include <sys/types.h>
     #include <sys/stat.h>])

]) # gl_HEADER_SYS_STAT_H
