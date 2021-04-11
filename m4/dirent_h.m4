# dirent_h.m4 serial 17
dnl Copyright (C) 2008-2021 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl Written by Bruno Haible.

AC_DEFUN([gl_DIRENT_H],
[
  dnl Use AC_REQUIRE here, so that the default behavior below is expanded
  dnl once only, before all statements that occur in other macros.
  AC_REQUIRE([gl_DIRENT_H_DEFAULTS])

  dnl <dirent.h> is always overridden, because of GNULIB_POSIXCHECK.
  gl_CHECK_NEXT_HEADERS([dirent.h])
  if test $ac_cv_header_dirent_h = yes; then
    HAVE_DIRENT_H=1
  else
    HAVE_DIRENT_H=0
  fi
  AC_SUBST([HAVE_DIRENT_H])

  dnl Check for declarations of anything we want to poison if the
  dnl corresponding gnulib module is not in use.
  gl_WARN_ON_USE_PREPARE([[#include <dirent.h>
    ]], [alphasort closedir dirfd fdopendir opendir readdir rewinddir scandir])
])

AC_DEFUN([gl_DIRENT_MODULE_INDICATOR],
[
  dnl Use AC_REQUIRE here, so that the default settings are expanded once only.
  AC_REQUIRE([gl_DIRENT_H_DEFAULTS])
  gl_MODULE_INDICATOR_SET_VARIABLE([$1])
  dnl Define it also as a C macro, for the benefit of the unit tests.
  gl_MODULE_INDICATOR_FOR_TESTS([$1])
])

AC_DEFUN([gl_DIRENT_H_DEFAULTS],
[
  AC_REQUIRE([gl_UNISTD_H_DEFAULTS]) dnl for REPLACE_FCHDIR
  gl_MODULE_INDICATOR_INIT_VARIABLE([GNULIB_OPENDIR])
  gl_MODULE_INDICATOR_INIT_VARIABLE([GNULIB_READDIR])
  gl_MODULE_INDICATOR_INIT_VARIABLE([GNULIB_REWINDDIR])
  gl_MODULE_INDICATOR_INIT_VARIABLE([GNULIB_CLOSEDIR])
  gl_MODULE_INDICATOR_INIT_VARIABLE([GNULIB_DIRFD])
  gl_MODULE_INDICATOR_INIT_VARIABLE([GNULIB_FDOPENDIR])
  gl_MODULE_INDICATOR_INIT_VARIABLE([GNULIB_SCANDIR])
  gl_MODULE_INDICATOR_INIT_VARIABLE([GNULIB_ALPHASORT])
  dnl Assume proper GNU behavior unless another module says otherwise.
  HAVE_OPENDIR=1;       AC_SUBST([HAVE_OPENDIR])
  HAVE_READDIR=1;       AC_SUBST([HAVE_READDIR])
  HAVE_REWINDDIR=1;     AC_SUBST([HAVE_REWINDDIR])
  HAVE_CLOSEDIR=1;      AC_SUBST([HAVE_CLOSEDIR])
  HAVE_DECL_DIRFD=1;    AC_SUBST([HAVE_DECL_DIRFD])
  HAVE_DECL_FDOPENDIR=1;AC_SUBST([HAVE_DECL_FDOPENDIR])
  HAVE_FDOPENDIR=1;     AC_SUBST([HAVE_FDOPENDIR])
  HAVE_SCANDIR=1;       AC_SUBST([HAVE_SCANDIR])
  HAVE_ALPHASORT=1;     AC_SUBST([HAVE_ALPHASORT])
  REPLACE_OPENDIR=0;    AC_SUBST([REPLACE_OPENDIR])
  REPLACE_CLOSEDIR=0;   AC_SUBST([REPLACE_CLOSEDIR])
  REPLACE_DIRFD=0;      AC_SUBST([REPLACE_DIRFD])
  REPLACE_FDOPENDIR=0;  AC_SUBST([REPLACE_FDOPENDIR])
])
