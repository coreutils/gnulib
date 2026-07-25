# posix_spawn_faction_addchdir.m4
# serial 3
dnl Copyright (C) 2018-2026 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN([gl_FUNC_POSIX_SPAWN_FILE_ACTIONS_ADDCHDIR],
[
  AC_REQUIRE([gl_SPAWN_H_DEFAULTS])
  AC_REQUIRE([AC_PROG_CC])
  gl_POSIX_SPAWN
  dnl Use AC_CHECK_DECLS, not AC_CHECK_FUNCS, because of glibc 2.44:
  dnl <https://sourceware.org/bugzilla/show_bug.cgi?id=34437>.
  AC_CHECK_DECLS([posix_spawn_file_actions_addchdir],,,
    [[#include <spawn.h>]])
  gl_CHECK_FUNCS_ANDROID([posix_spawn_file_actions_addchdir_np],
    [[#include <spawn.h>]])
  if test $ac_cv_have_decl_posix_spawn_file_actions_addchdir = yes; then
    dnl This function is not yet standardized. Therefore override the
    dnl system's implementation always.
    REPLACE_POSIX_SPAWN_FILE_ACTIONS_ADDCHDIR=1
  else
    HAVE_POSIX_SPAWN_FILE_ACTIONS_ADDCHDIR=0
  fi
])
