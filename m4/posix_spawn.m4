# posix_spawn.m4 serial 1
dnl Copyright (C) 2008 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl Tests whether the entire posix_spawn facility is available.
AC_DEFUN([gl_POSIX_SPAWN],
[
  AC_REQUIRE([gl_POSIX_SPAWN_BODY])
])

AC_DEFUN([gl_POSIX_SPAWN_BODY],
[
  AC_REQUIRE([gl_SPAWN_H_DEFAULTS])
  AC_CHECK_FUNCS_ONCE([posix_spawn])
  dnl Assume that when the main function exists, all the others are
  dnl available as well.
  dnl AC_CHECK_FUNCS_ONCE([posix_spawnp])
  dnl AC_CHECK_FUNCS_ONCE([posix_spawn_file_actions_init])
  dnl AC_CHECK_FUNCS_ONCE([posix_spawn_file_actions_addclose])
  dnl AC_CHECK_FUNCS_ONCE([posix_spawn_file_actions_adddup2])
  dnl AC_CHECK_FUNCS_ONCE([posix_spawn_file_actions_addopen])
  dnl AC_CHECK_FUNCS_ONCE([posix_spawn_file_actions_destroy])
  dnl AC_CHECK_FUNCS_ONCE([posix_spawnattr_init])
  dnl AC_CHECK_FUNCS_ONCE([posix_spawnattr_getflags])
  dnl AC_CHECK_FUNCS_ONCE([posix_spawnattr_setflags])
  dnl AC_CHECK_FUNCS_ONCE([posix_spawnattr_getpgroup])
  dnl AC_CHECK_FUNCS_ONCE([posix_spawnattr_setpgroup])
  dnl AC_CHECK_FUNCS_ONCE([posix_spawnattr_getschedparam])
  dnl AC_CHECK_FUNCS_ONCE([posix_spawnattr_setschedparam])
  dnl AC_CHECK_FUNCS_ONCE([posix_spawnattr_getschedpolicy])
  dnl AC_CHECK_FUNCS_ONCE([posix_spawnattr_setschedpolicy])
  dnl AC_CHECK_FUNCS_ONCE([posix_spawnattr_getsigdefault])
  dnl AC_CHECK_FUNCS_ONCE([posix_spawnattr_setsigdefault])
  dnl AC_CHECK_FUNCS_ONCE([posix_spawnattr_getsigmask])
  dnl AC_CHECK_FUNCS_ONCE([posix_spawnattr_setsigmask])
  dnl AC_CHECK_FUNCS_ONCE([posix_spawnattr_destroy])
  if test $ac_cv_func_posix_spawn != yes; then
    HAVE_POSIX_SPAWN=0
    dnl For now, assume that if posix_spawn exists, it also works.
    if false; then
      REPLACE_POSIX_SPAWN=1
    fi
  fi
])

AC_DEFUN([gl_POSIX_SPAWN_INTERNAL],
[
  AC_LIBOBJ([spawni])
  dnl Prerequisites of lib/spawni.c.
  AC_CHECK_HEADERS([paths.h])
  AC_CHECK_FUNCS([confstr sched_setparam sched_setscheduler setegid seteuid vfork])
])
