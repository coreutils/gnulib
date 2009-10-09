# serial 1
# See if we need to provide futimens replacement.

dnl Copyright (C) 2009 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

# Written by Eric Blake.

AC_DEFUN([gl_FUNC_FUTIMENS],
[
  AC_REQUIRE([gl_SYS_STAT_H_DEFAULTS])
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])
  AC_CHECK_FUNCS_ONCE([futimens])
  if test $ac_cv_func_futimens = no; then
    HAVE_FUTIMENS=0
    AC_LIBOBJ([futimens])
  else
    AC_CACHE_CHECK([whether futimens works],
      [gl_cv_func_futimens_works],
      [AC_RUN_IFELSE([AC_LANG_PROGRAM([[
#include <fcntl.h>
#include <sys/stat.h>
#ifdef __linux__
/* The Linux kernel added futimens in 2.6.22, but it had bugs until 2.6.26.
   Always replace futimens to support older kernels.  */
choke me
#endif
]], [[struct timespec ts[2] = { { 1, UTIME_OMIT }, { 1, UTIME_OMIT } };
      if (futimens (AT_FDCWD, NULL)) return 1;
      return futimens (open (".", O_RDONLY), ts);]])],
         [gl_cv_func_futimens_works=yes],
         [gl_cv_func_futimens_works="needs runtime check"],
         [gl_cv_func_futimens_works="guessing no"])])
    if test "$gl_cv_func_futimens_works" != yes; then
      REPLACE_FUTIMENS=1
      AC_LIBOBJ([futimens])
    fi
  fi
])
