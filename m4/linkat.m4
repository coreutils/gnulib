# serial 2
# See if we need to provide linkat replacement.

dnl Copyright (C) 2009-2010 Free Software Foundation, Inc.
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
  else
    AC_CACHE_CHECK([whether linkat(,AT_SYMLINK_FOLLOW) works],
      [gl_cv_func_linkat_follow],
      [rm -rf conftest.f1 conftest.f2
       touch conftest.f1
       AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <fcntl.h>
#include <unistd.h>
#ifdef __linux__
/* Linux added linkat in 2.6.16, but did not add AT_SYMLINK_FOLLOW
   until 2.6.18.  Always replace linkat to support older kernels.  */
choke me
#endif
]], [return linkat (AT_FDCWD, "conftest.f1", AT_FDCWD, "conftest.f2",
                    AT_SYMLINK_FOLLOW);])],
         [gl_cv_func_linkat_follow=yes],
         [gl_cv_func_linkat_follow="need runtime check"])
       rm -rf conftest.f1 conftest.f2])
    if test "$gl_cv_func_linkat_follow" != yes; then
      REPLACE_LINKAT=1
      AC_LIBOBJ([linkat])
    fi
  fi
])
