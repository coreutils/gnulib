# unlink.m4 serial 3
dnl Copyright (C) 2009, 2010 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_UNLINK],
[
  AC_REQUIRE([gl_AC_DOS])
  AC_REQUIRE([gl_UNISTD_H_DEFAULTS])
  dnl Detect Solaris 9 and FreeBSD 7.2 bug.
  AC_CACHE_CHECK([whether unlink honors trailing slashes],
    [gl_cv_func_unlink_works],
    [touch conftest.file
     # Assume that if we have lstat, we can also check symlinks.
     if test $ac_cv_func_lstat = yes; then
       ln -s conftest.file conftest.lnk
     fi
     AC_RUN_IFELSE(
       [AC_LANG_PROGRAM(
         [[#include <unistd.h>
           #include <errno.h>
]], [[if (!unlink ("conftest.file/") || errno != ENOTDIR) return 1;
#if HAVE_LSTAT
      if (!unlink ("conftest.lnk/") || errno != ENOTDIR) return 2;
#endif
      ]])],
      [gl_cv_func_unlink_works=yes], [gl_cv_func_unlink_works=no],
      [gl_cv_func_unlink_works="guessing no"])
     rm -f conftest.file conftest.lnk])
  if test x"$gl_cv_func_unlink_works" != xyes; then
    REPLACE_UNLINK=1
    AC_LIBOBJ([unlink])
  fi
])
