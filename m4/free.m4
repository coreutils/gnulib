# free.m4 serial 3
# Copyright (C) 2003-2005, 2009-2020 Free Software Foundation, Inc.
# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# Written by Paul Eggert.

AC_DEFUN([gl_FUNC_FREE],
[
  AC_REQUIRE([gl_STDLIB_H_DEFAULTS])

  dnl In the next release of POSIX, free must preserve errno.
  dnl https://www.austingroupbugs.net/view.php?id=385
  dnl https://sourceware.org/bugzilla/show_bug.cgi?id=17924
  dnl For now, assume implementations other than glibc do not preserve errno
  dnl unless they set _POSIX_VERSION to the next release number,
  dnl whatever that happens to be.
  AC_CACHE_CHECK([whether free is known to preserve errno],
    [gl_cv_func_free_preserves_errno],
    [AC_COMPILE_IFELSE(
       [AC_LANG_PROGRAM(
          [[#include <unistd.h>
          ]],
          [[#if _POSIX_VERSION <= 200809
              #error "'free' is not known to preserve errno"
            #endif
          ]])],
       [gl_cv_func_free_preserves_errno=yes],
       [gl_cv_func_free_preserves_errno=no])
    ])

  case $gl_cv_func_free_preserves_errno in
   *yes) ;;
   *) REPLACE_FREE=1 ;;
  esac
])

# Prerequisites of lib/free.c.
AC_DEFUN([gl_PREREQ_FREE], [:])
