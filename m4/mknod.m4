# serial 3
# See if we need to provide mknod replacement.

dnl Copyright (C) 2009-2011 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

# Written by Eric Blake.

AC_DEFUN([gl_FUNC_MKNOD],
[
  AC_REQUIRE([gl_SYS_STAT_H_DEFAULTS])
  AC_REQUIRE([gl_FUNC_MKFIFO])
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])
  AC_CHECK_FUNCS_ONCE([mknod])
  if test $ac_cv_func_mknod = no; then
    HAVE_MKNOD=0
    AC_LIBOBJ([mknod])
  else
    dnl Detect BSD bug, where mknod requires root privileges to create fifo.
    AC_CACHE_CHECK([whether mknod can create fifo without root privileges],
      [gl_cv_func_mknod_works],
      [AC_RUN_IFELSE(
         [AC_LANG_PROGRAM(
           [[#include <sys/stat.h>
             #include <unistd.h>
]], [[/* Indeterminate for super-user, assume no.  Why are you running
         configure as root, anyway?  */
      if (!geteuid ()) return 99;
      if (mknod ("conftest.fifo", S_IFIFO | 0600, 0)) return 2;]])],
         [gl_cv_func_mknod_works=yes],
         [if test $? = 99 && test x"$FORCE_UNSAFE_CONFIGURE" = x; then
            AC_MSG_FAILURE([you should not run configure as root ]dnl
[(set FORCE_UNSAFE_CONFIGURE=1 in environment to bypass this check)])
          fi
          gl_cv_func_mknod_works=no],
         [gl_cv_func_mknod_works="guessing no"])
       rm -f conftest.fifo])
    if test "$gl_cv_func_mknod_works" != yes; then
      AC_DEFINE([MKNOD_FIFO_BUG], [1], [Define to 1 if mknod cannot create
        a fifo without super-user privileges])
    fi
    dnl Systems that mishandle trailing slash on mkfifo also goof on mknod.
    if test $REPLACE_MKFIFO = 1 || test "$gl_cv_func_mknod_works" != yes; then
      REPLACE_MKNOD=1
      AC_LIBOBJ([mknod])
    fi
  fi
])
