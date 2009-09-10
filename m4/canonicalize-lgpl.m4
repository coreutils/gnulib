# canonicalize-lgpl.m4 serial 8
dnl Copyright (C) 2003, 2006-2007, 2009 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_CANONICALIZE_LGPL],
[
  dnl Do this replacement check manually because the file name is shorter
  dnl than the function name.
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])
  AC_CHECK_FUNCS_ONCE([canonicalize_file_name realpath])
  dnl Assume that all platforms with canonicalize_file_name also have
  dnl a working realpath.
  if test $ac_cv_func_canonicalize_file_name = no; then
    HAVE_CANONICALIZE_FILE_NAME=0
    AC_LIBOBJ([canonicalize-lgpl])
    if test $ac_cv_func_realpath = no; then
      HAVE_REALPATH=0
    else
      AC_CACHE_CHECK([whether realpath works], [gl_cv_func_realpath_works], [
        touch conftest.a
        AC_RUN_IFELSE([
          AC_LANG_PROGRAM([[
            #include <stdlib.h>
          ]], [[
            char *name1 = realpath ("conftest.a", NULL);
            char *name2 = realpath ("conftest.b/../conftest.a", NULL);
            return !(name1 && *name1 == '/' && !name2);
          ]])
        ], [gl_cv_func_realpath_works=yes], [gl_cv_func_realpath_works=no],
           [gl_cv_func_realpath_works="guessing no"])
      ])
      if test $gl_cv_func_realpath_works != yes; then
        REPLACE_REALPATH=1
      else
        AC_DEFINE([FUNC_REALPATH_WORKS], [1], [Define to 1 if realpath()
          can malloc memory and always gives an absolute path.])
      fi
    fi
    gl_PREREQ_CANONICALIZE_LGPL
  fi
])

# Like gl_CANONICALIZE_LGPL, except prepare for separate compilation
# (no AC_LIBOBJ).
AC_DEFUN([gl_CANONICALIZE_LGPL_SEPARATE],
[
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])
  AC_CHECK_FUNCS_ONCE([canonicalize_file_name])
  gl_PREREQ_CANONICALIZE_LGPL
])

# Prerequisites of lib/canonicalize-lgpl.c.
AC_DEFUN([gl_PREREQ_CANONICALIZE_LGPL],
[
  AC_CHECK_HEADERS_ONCE([sys/param.h])
  AC_CHECK_FUNCS_ONCE([getcwd readlink])
])
