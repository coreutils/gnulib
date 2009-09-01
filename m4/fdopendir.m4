# serial 1
# See if we need to provide fdopendir.

dnl Copyright (C) 2009 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

# Written by Eric Blake.

AC_DEFUN([gl_FUNC_FDOPENDIR],
[
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])
  AC_CHECK_FUNCS_ONCE([fdopendir])
  if test $ac_cv_func_fdopendir = no; then
    AC_LIBOBJ([openat-proc])
    AC_LIBOBJ([fdopendir])
    gl_REPLACE_DIRENT_H
    HAVE_FDOPENDIR=0
  fi
])
