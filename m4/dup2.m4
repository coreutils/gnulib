#serial 6
dnl Copyright (C) 2002, 2005, 2007, 2009 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_DUP2],
[
  AC_REQUIRE([gl_UNISTD_H_DEFAULTS])
  AC_REQUIRE([AC_CANONICAL_HOST])
  AC_CHECK_FUNCS_ONCE([dup2])
  if test $ac_cv_func_dup2 = no; then
    HAVE_DUP2=0
    AC_LIBOBJ([dup2])
  else
    AC_CACHE_CHECK([whether dup2 works], [gl_cv_func_dup2_works],
      [AC_RUN_IFELSE([AC_LANG_PROGRAM([[
#include <unistd.h>
]], [return 1 - dup2 (1, 1);])],
	[gl_cv_func_dup2_works=yes], [gl_cv_func_dup2_works=no],
	[case "$host_os" in
	   mingw*) # on this platform, dup2 always returns 0 for success
	     gl_cv_func_dup2_works=no;;
	   cygwin*) # on cygwin 1.5.x, dup2(1,1) returns 0
	     gl_cv_func_dup2_works=no;;
	   *) gl_cv_func_dup2_works=yes;;
	 esac])])
    if test "$gl_cv_func_dup2_works" = no; then
      REPLACE_DUP2=1
      AC_LIBOBJ([dup2])
    fi
  fi
  AC_DEFINE_UNQUOTED([REPLACE_DUP2], [$REPLACE_DUP2],
    [Define to 1 if dup2 returns 0 instead of the target fd.])
])
