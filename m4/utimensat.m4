# serial 3
# See if we need to provide utimensat replacement.

dnl Copyright (C) 2009, 2010 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

# Written by Eric Blake.

AC_DEFUN([gl_FUNC_UTIMENSAT],
[
  AC_REQUIRE([gl_SYS_STAT_H_DEFAULTS])
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])
  AC_CHECK_FUNCS_ONCE([utimensat])
  if test $ac_cv_func_utimensat = no; then
    HAVE_UTIMENSAT=0
    AC_LIBOBJ([utimensat])
  else
    AC_CACHE_CHECK([whether utimensat works],
      [gl_cv_func_utimensat_works],
      [AC_RUN_IFELSE([AC_LANG_PROGRAM([[
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
]], [[struct timespec ts[2] = { { 1, UTIME_OMIT }, { 1, UTIME_NOW } };
      struct stat st;
      const char *f = "conftest.file";
      if (close (creat (f, 0600))) return 1;
      if (utimensat (AT_FDCWD, f, NULL, AT_SYMLINK_NOFOLLOW)) return 2;
      if (utimensat (AT_FDCWD, f, ts, 0)) return 3;
      sleep (1);
      ts[0].tv_nsec = UTIME_NOW;
      ts[1].tv_nsec = UTIME_OMIT;
      if (utimensat (AT_FDCWD, f, ts, 0)) return 4;
      if (stat (f, &st)) return 5;
      if (st.st_ctime < st.st_atime) return 6;]])],
dnl FIXME: simplify this in 2012, when file system bugs are no longer common
         [AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#ifdef __linux__
/* The Linux kernel added utimensat in 2.6.22, but has bugs with UTIME_OMIT
   in several file systems as recently as 2.6.32.  Always replace utimensat
   to support older kernels.  */
choke me
#endif
      ]])],
           [gl_cv_func_utimensat_works=yes],
           [gl_cv_func_utimensat_works="needs runtime check"])],
         [gl_cv_func_utimensat_works=no],
         [gl_cv_func_utimensat_works="guessing no"])])
    if test "$gl_cv_func_utimensat_works" != yes; then
      REPLACE_UTIMENSAT=1
      AC_LIBOBJ([utimensat])
    fi
  fi
])
