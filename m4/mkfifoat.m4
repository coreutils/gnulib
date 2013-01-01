# serial 3
# See if we need to provide mkfifoat/mknodat replacement.

dnl Copyright (C) 2009-2013 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

# Written by Eric Blake.

AC_DEFUN([gl_FUNC_MKFIFOAT],
[
  AC_REQUIRE([gl_SYS_STAT_H_DEFAULTS])
  AC_REQUIRE([gl_FUNC_OPENAT])
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])
  AC_CHECK_FUNCS_ONCE([mkfifoat mknodat])
  if test $ac_cv_func_mkfifoat = no; then
    # No known system has mkfifoat but not mknodat
    HAVE_MKFIFOAT=0
    HAVE_MKNODAT=0
  fi
])
