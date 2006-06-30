# sys_stat_h.m4 serial 1   -*- Autoconf -*-
dnl Copyright (C) 2006 Free Software Foundation, Inc.
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

  dnl Check for mkdir.  Mingw has _mkdir(name) in the nonstandard <io.h>
  dnl instead.
  AC_CHECK_DECLS([mkdir], [], [], [#include <sys/stat.h>])

  SYS_STAT_H=
  if test $ac_cv_func_lstat:$ac_cv_have_decl_mkdir != yes:yes ; then
    gl_FULL_HEADER_PATH([sys/stat.h])
    FULL_PATH_SYS_STAT_H='<'$gl_cv_full_path_sys_stat_h'>'
    AC_CHECK_HEADERS([io.h])
    AC_REQUIRE([AC_C_INLINE])
    AC_SUBST([FULL_PATH_SYS_STAT_H])
    SYS_STAT_H='sys/stat.h'
  fi
  AC_SUBST([SYS_STAT_H])
]) # gl_HEADER_SYS_STAT_H
