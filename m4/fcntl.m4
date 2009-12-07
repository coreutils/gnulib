# fcntl.m4 serial 1
dnl Copyright (C) 2009 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

# For now, this module ensures that fcntl()
# - supports or emulates F_DUPFD_CLOEXEC
# Still to be ported to various platforms:
# - supports F_DUPFD correctly
# Still to be ported to mingw:
# - F_GETFD, F_SETFD, F_DUPFD
# - F_DUPFD_CLOEXEC
# - F_GETFL, F_SETFL
# - F_GETOWN, F_SETOWN
# - F_GETLK, F_SETLK, F_SETLKW
AC_DEFUN([gl_FUNC_FCNTL],
[
  dnl Persuade glibc to expose F_DUPFD_CLOEXEC.
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])
  AC_REQUIRE([gl_FCNTL_H_DEFAULTS])
  AC_CHECK_FUNCS_ONCE([fcntl])
  if test $ac_cv_func_fcntl = no; then
    HAVE_FCNTL=0
  else
    AC_CACHE_CHECK([whether fcntl understands F_DUPFD_CLOEXEC],
      [gl_cv_func_fcntl_f_dupfd_cloexec],
      [AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <fcntl.h>
#ifndef F_DUPFD_CLOEXEC
choke me
#endif
         ]])],
         [AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#ifdef __linux__
/* The Linux kernel only added F_DUPFD_CLOEXEC in 2.6.24, so we always replace
   it to support the semantics on older kernels that failed with EINVAL.  */
choke me
#endif
           ]])],
           [gl_cv_func_fcntl_f_dupfd_cloexec=yes],
           [gl_cv_func_fcntl_f_dupfd_cloexec="needs runtime check"])],
         [gl_cv_func_fcntl_f_dupfd_cloexec=no])])
    if test "$gl_cv_func_fcntl_f_dupfd_cloexec" != yes; then
      REPLACE_FCNTL=1
      AC_LIBOBJ([fcntl])
    fi
  fi
])
