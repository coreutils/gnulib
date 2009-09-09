# serial 1
# See if we need to provide linkat replacement.

dnl Copyright (C) 2009 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

# Written by Eric Blake.

AC_DEFUN([gl_FUNC_LINKAT],
[
  AC_REQUIRE([gl_FUNC_OPENAT])
  AC_REQUIRE([gl_FUNC_LINK])
  AC_REQUIRE([gl_FUNC_LINK_FOLLOWS_SYMLINK])
  AC_REQUIRE([gl_UNISTD_H_DEFAULTS])
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])
  AC_CHECK_FUNCS_ONCE([linkat symlink])
  AC_CHECK_HEADERS_ONCE([sys/param.h])
  if test $ac_cv_func_linkat = no; then
    HAVE_LINKAT=0
    AC_LIBOBJ([linkat])
    AC_LIBOBJ([at-func2])
  fi
])
