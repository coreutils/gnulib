# serial 3
# See if we need to provide symlinkat/readlinkat replacement.

dnl Copyright (C) 2009-2010 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

# Written by Eric Blake.

AC_DEFUN([gl_FUNC_SYMLINKAT],
[
  AC_REQUIRE([gl_FUNC_OPENAT])
  AC_REQUIRE([gl_UNISTD_H_DEFAULTS])
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])
  AC_CHECK_FUNCS_ONCE([symlinkat readlinkat])
  if test $ac_cv_func_symlinkat = no; then
    # No known system has readlinkat but not symlinkat
    HAVE_SYMLINKAT=0
    HAVE_READLINKAT=0
    AC_LIBOBJ([symlinkat])
  fi
])
