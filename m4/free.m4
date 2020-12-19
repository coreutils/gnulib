# free.m4 serial 4
# Copyright (C) 2003-2005, 2009-2020 Free Software Foundation, Inc.
# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# Written by Paul Eggert and Bruno Haible.

AC_DEFUN([gl_FUNC_FREE],
[
  AC_REQUIRE([gl_STDLIB_H_DEFAULTS])
  AC_REQUIRE([AC_CANONICAL_HOST])

  dnl In the next release of POSIX, free must preserve errno.
  dnl https://www.austingroupbugs.net/view.php?id=385
  dnl https://sourceware.org/bugzilla/show_bug.cgi?id=17924
  dnl So far, we know of two platforms that do this:
  dnl * OpenBSD >= 4.5, thanks to this commit:
  dnl   <https://cvsweb.openbsd.org/cgi-bin/cvsweb/src/lib/libc/stdlib/malloc.c.diff?r1=1.100&r2=1.101&f=h>
  dnl * Solaris, because its malloc() implementation is based on brk(),
  dnl   not mmap(); hence its free() implementation makes no system calls.
  dnl For other platforms, you can only be sure if they state it in their
  dnl documentation, or by code inspection of the free() implementation in libc.
  AC_CACHE_CHECK([whether free is known to preserve errno],
    [gl_cv_func_free_preserves_errno],
    [case "$host_os" in
       # Say yes only if we know it.
       openbsd* | solaris*)
         gl_cv_func_free_preserves_errno=yes
         ;;
       # It's no on Linux, for implementations that call munmap(), due to
       # /proc/sys/vm/max_map_count.
       linux*)
         gl_cv_func_free_preserves_errno=no
         ;;
       # If we don't know, obey --enable-cross-guesses.
       *)
         gl_cv_func_free_preserves_errno="$gl_cross_guess_normal"
         ;;
     esac
    ])

  case $gl_cv_func_free_preserves_errno in
   *yes) ;;
   *) REPLACE_FREE=1 ;;
  esac
])

# Prerequisites of lib/free.c.
AC_DEFUN([gl_PREREQ_FREE], [:])
