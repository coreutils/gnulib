# serial 5
# See if we need to provide fdopendir.

dnl Copyright (C) 2009-2011 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

# Written by Eric Blake.

AC_DEFUN([gl_FUNC_FDOPENDIR],
[
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])
  dnl FreeBSD 7.3 has the function, but failed to declare it.
  AC_CHECK_DECLS([fdopendir], [], [HAVE_DECL_FDOPENDIR=0], [[
#include <dirent.h>
    ]])
  AC_CHECK_FUNCS_ONCE([fdopendir])
  if test $ac_cv_func_fdopendir = no; then
    AC_LIBOBJ([openat-proc])
    AC_LIBOBJ([fdopendir])
    gl_REPLACE_DIRENT_H
    HAVE_FDOPENDIR=0
  else
    AC_CACHE_CHECK([whether fdopendir works],
      [gl_cv_func_fdopendir_works],
      [AC_RUN_IFELSE([AC_LANG_PROGRAM([[
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#if !HAVE_DECL_FDOPENDIR
extern DIR *fdopendir (int);
#endif
]], [int result = 0;
     int fd = open ("conftest.c", O_RDONLY);
     if (fd < 0) result |= 1;
     if (fdopendir (fd)) result |= 2;
     if (close (fd)) result |= 4;
     return result;])],
         [gl_cv_func_fdopendir_works=yes],
         [gl_cv_func_fdopendir_works=no],
         [gl_cv_func_fdopendir_works="guessing no"])])
    if test "$gl_cv_func_fdopendir_works" != yes; then
      REPLACE_FDOPENDIR=1
      gl_REPLACE_DIRENT_H
      AC_LIBOBJ([fdopendir])
    fi
  fi
])
