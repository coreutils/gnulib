# serial 1
# See if we need to provide symlink replacement.

dnl Copyright (C) 2009 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

# Written by Eric Blake.

AC_DEFUN([gl_FUNC_SYMLINK],
[
  AC_REQUIRE([gl_UNISTD_H_DEFAULTS])
  AC_CHECK_FUNCS_ONCE([symlink])
  dnl The best we can do on mingw is provide a dummy that always fails, so
  dnl that compilation can proceed with fewer ifdefs.  On Solaris 9, we
  dnl want to fix a bug with trailing slash handling.
  if test $ac_cv_func_symlink = no; then
    HAVE_SYMLINK=0
    AC_LIBOBJ([symlink])
  else
    AC_CACHE_CHECK([whether symlink handles trailing slash correctly],
      [gl_cv_func_symlink_works],
      [AC_RUN_IFELSE(
         [AC_LANG_PROGRAM(
           [[#include <unistd.h>
]], [[return !symlink ("a", "conftest.link/");]])],
         [gl_cv_func_symlink_works=yes], [gl_cv_func_symlink_works=no],
         [gl_cv_func_symlink_works="guessing no"])])
    if test "$gl_cv_func_symlink_works" != yes; then
      REPLACE_SYMLINK=1
      AC_LIBOBJ([symlink])
    fi
  fi
])
