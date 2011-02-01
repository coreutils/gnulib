# ioctl.m4 serial 1
dnl Copyright (C) 2008-2011 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_IOCTL],
[
  AC_REQUIRE([gl_SYS_IOCTL_H_DEFAULTS])
  AC_REQUIRE([gl_HEADER_SYS_SOCKET])
  if test "$ac_cv_header_winsock2_h" = yes; then
    dnl Even if the 'socket' module is not used here, another part of the
    dnl application may use it and pass file descriptors that refer to
    dnl sockets to the ioctl() function. So enable the support for sockets.
    AC_LIBOBJ([ioctl])
    gl_REPLACE_SYS_IOCTL_H
  else
    AC_CHECK_FUNCS([ioctl])
    dnl On glibc systems, the second parameter is 'unsigned long int request',
    dnl not 'int request'. We cannot simply cast the function pointer, but
    dnl instead need a wrapper.
    AC_CACHE_CHECK([for ioctl with POSIX signature],
      [gl_cv_func_ioctl_posix_signature],
      [AC_COMPILE_IFELSE(
         [AC_LANG_PROGRAM(
            [[#include <sys/ioctl.h>]],
            [[extern int ioctl (int, int, ...);]])
         ],
         [gl_cv_func_ioctl_posix_signature=yes],
         [gl_cv_func_ioctl_posix_signature=no])
      ])
    if test $gl_cv_func_ioctl_posix_signature != yes; then
      REPLACE_IOCTL=1
      AC_LIBOBJ([ioctl])
      gl_REPLACE_SYS_IOCTL_H
    fi
  fi
])
