# serial 14

dnl From Jim Meyering.
dnl A wrapper around AC_FUNC_GETGROUPS.

# Copyright (C) 1996-1997, 1999-2004, 2008-2009 Free Software Foundation, Inc.
#
# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_GETGROUPS],
[
  AC_REQUIRE([AC_FUNC_GETGROUPS])
  AC_REQUIRE([AC_TYPE_GETGROUPS])
  AC_REQUIRE([gl_UNISTD_H_DEFAULTS])
  if test "$ac_cv_func_getgroups" != yes; then
    AC_LIBOBJ([getgroups])
    HAVE_GETGROUPS=0
  elif test "$ac_cv_func_getgroups_works.$ac_cv_type_getgroups" != yes.gid_t
  then
    AC_LIBOBJ([getgroups])
    REPLACE_GETGROUPS=1
  fi
  test -n "$GETGROUPS_LIB" && LIBS="$GETGROUPS_LIB $LIBS"
])
