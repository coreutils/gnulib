# serial 4

# Copyright (C) 2006, 2009 Free Software Foundation, Inc.
# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# Derived from rename.m4.

# A rename wrapper to make tools like mv -- that would normally
# rely on the underlying rename syscall -- work more consistently.

AC_DEFUN([gl_FUNC_RENAME_TRAILING_DEST_SLASH],
[
 AC_CACHE_CHECK([whether rename is broken with respect to destination slashes],
  gl_cv_func_rename_trailing_dest_slash_bug,
  [
    rm -rf conftest.d1 conftest.d2
    mkdir conftest.d1 ||
      AC_MSG_ERROR([cannot create temporary directory])
    AC_RUN_IFELSE([AC_LANG_SOURCE([[
#       include <stdio.h>
        int
        main ()
        {
          return (rename ("conftest.d1", "conftest.d2/") ? 1 : 0);
        }
      ]])],
      [gl_cv_func_rename_trailing_dest_slash_bug=no],
      [gl_cv_func_rename_trailing_dest_slash_bug=yes],
      dnl When crosscompiling, assume rename is broken.
      [gl_cv_func_rename_trailing_dest_slash_bug=yes])

      rm -rf conftest.d1 conftest.d2
  ])
  if test $gl_cv_func_rename_trailing_dest_slash_bug = yes; then
    AC_LIBOBJ([rename-dest-slash])
    AC_DEFINE([rename], [rpl_rename_dest_slash],
      [Define to rpl_rename_dest_slash if the replacement function should be used.])
  fi
])
