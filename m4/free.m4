# free.m4 serial 1
# Copyright (C) 2003-2005, 2009-2020 Free Software Foundation, Inc.
# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# Written by Paul Eggert.

# We can't test for free (NULL) even at runtime, since it might
# happen to "work" for our test program, but not in general.  So, be
# conservative and use feature tests for relatively modern hosts,
# where free (NULL) is known to work.  This costs a bit of
# performance on some older hosts, but we can fix that later if
# needed.

AC_DEFUN([gl_FUNC_FREE],
[
  AC_REQUIRE([gl_STDLIB_H_DEFAULTS])
  AC_REQUIRE([AC_CANONICAL_HOST])
  AC_CACHE_CHECK([whether free (NULL) is known to work],
    [gl_cv_func_free],
    [case "$host_os" in
       mingw*) gl_cv_func_free=yes ;;
       *)
         AC_COMPILE_IFELSE(
           [AC_LANG_PROGRAM(
              [[#include <unistd.h>]],
              [[#if _POSIX_VERSION < 199009L && \
                    (defined unix || defined _unix || defined _unix_ \
                     || defined __unix || defined __unix__)
                  #error "'free (NULL)' is not known to work"
                #endif]])],
           [gl_cv_func_free=yes],
           [gl_cv_func_free=no])
     esac
    ])

  dnl In the next release of POSIX, free must preserve errno.
  dnl https://www.austingroupbugs.net/view.php?id=385
  dnl https://sourceware.org/bugzilla/show_bug.cgi?id=17924
  dnl For now, assume implementations other than glibc do not preserve errno
  dnl unless they set _POSIX_VERSION to the next release number,
  dnl whatever that happens to be.
  AC_CACHE_CHECK([whether free is known to preserve errno],
    [gl_cv_func_free_preserves_errno],
    [case $host_os in
       *-gnu* | gnu*)
         gl_cv_func_free_preserves_errno=yes;;
       *)
         AC_COMPILE_IFELSE(
           [AC_LANG_PROGRAM(
              [[#include <unistd.h>
              ]],
              [[#if _POSIX_VERSION <= 200809
                  #error "'free' is not known to preserve errno"
                #endif
              ]])],
           [gl_cv_func_free_preserves_errno=yes],
           [gl_cv_func_free_preserves_errno=no]);;
     esac
    ])

  if test $gl_cv_func_free = no; then
    AC_DEFINE([CANNOT_FREE_NULL], [1],
      [Define to 1 if free (NULL) does not work.])
  fi

  case $gl_cv_func_free,$gl_cv_func_free_preserves_errno in
   *yes,*yes) ;;
   *) REPLACE_FREE=1 ;;
  esac
])

# Prerequisites of lib/free.c.
AC_DEFUN([gl_PREREQ_FREE], [:])
