# libsigsegv.m4 serial 3
dnl Copyright (C) 2002-2003, 2008, 2009 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl From Bruno Haible, Sam Steingold.

AC_DEFUN([gl_LIBSIGSEGV],
[
  dnl Prerequisites of AC_LIB_LINKFLAGS_BODY.
  AC_REQUIRE([AC_LIB_PREPARE_PREFIX])
  AC_REQUIRE([AC_LIB_RPATH])

  dnl Search for libsigsegv and define LIBSIGSEGV, LTLIBSIGSEGV and INCSIGSEGV
  dnl accordingly.
  AC_LIB_LINKFLAGS_BODY([sigsegv])

  dnl Add $INCSIGSEGV to CPPFLAGS before performing the following checks,
  dnl because if the user has installed libsigsegv and not disabled its use
  dnl via --without-libsigsegv-prefix, he wants to use it.
  gl_save_CPPFLAGS="$CPPFLAGS"
  AC_LIB_APPENDTOVAR([CPPFLAGS], [$INCSIGSEGV])

  AC_CACHE_CHECK([for libsigsegv], [gl_cv_lib_sigsegv], [
    gl_cv_lib_sigsegv="no, consider installing GNU libsigsegv"
    gl_save_LIBS="$LIBS"
    LIBS="$LIBS $LIBSIGSEGV"
    AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <sigsegv.h>]],
      [sigsegv_deinstall_handler();])],
      [gl_cv_lib_sigsegv=yes])
    LIBS="$gl_save_LIBS"
  ])
  if test "$gl_cv_lib_sigsegv" = yes; then
    AC_DEFINE([HAVE_LIBSIGSEGV], [1],
      [Define if you have the libsigsegv library.])
    AC_MSG_CHECKING([how to link with libsigsegv])
    AC_MSG_RESULT([$LIBSIGSEGV])
  else
    dnl If $LIBSIGSEGV didn't lead to a usable library, we don't need
    dnl $INCSIGSEGV either.
    CPPFLAGS="$gl_save_CPPFLAGS"
    LIBSIGSEGV=
    LTLIBSIGSEGV=
  fi
  AC_SUBST([LIBSIGSEGV])
  AC_SUBST([LTLIBSIGSEGV])
])
