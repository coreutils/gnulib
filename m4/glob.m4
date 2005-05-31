# glob.m4 serial 1
dnl Copyright (C) 2005 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

# The glob module assumes you want GNU glob, with glob_pattern_p etc,
# rather than vanilla POSIX glob.  This means your your code should
# always include <glob.h> for the glob prototypes.

AC_DEFUN([gl_GLOB_SUBSTITUTE],
[
  gl_PREREQ_GLOB

  GLOB_H=glob.h
  AC_LIBOBJ([glob])
  AC_DEFINE([GLOB_PREFIX], [[rpl_]],
    [Define to rpl_ if the glob replacement functions and variables
     should be used.])
  AC_SUBST([GLOB_H])
])

AC_DEFUN([gl_GLOB],
[ GLOB_H=
  AC_CHECK_HEADERS([glob.h], [], [GLOB_H=glob.h])

  if test -z "$GLOB_H"; then
    AC_COMPILE_IFELSE(
[[#include <gnu-versions.h>
char a[_GNU_GLOB_INTERFACE_VERSION == 1 ? 1 : -1];]],
      [], [GLOB_H=glob.h])
  fi

  if test -n "$GLOB_H"; then
    gl_GLOB_SUBSTITUTE
  fi
])

# Prerequisites of lib/glob.*.
AC_DEFUN([gl_PREREQ_GLOB],
[ AC_REQUIRE([gl_CHECK_TYPE_STRUCT_DIRENT_D_TYPE])dnl
  AC_REQUIRE([AC_HEADER_DIRENT])dnl
  AC_CHECK_HEADERS_ONCE([sys/cdefs.h unistd.h])dnl
  AC_CHECK_FUNCS_ONCE([getlogin_r getpwnam_r])dnl
  AC_CHECK_HEADERS([sys/cdefs.h], [SYS_CDEFS_H=yes], [SYS_CDEFS_H=no])

  dnl Note the reversal of the common HAVE_SYS_CDEFS_H idiom below.  In this
  dnl way, #ifndef _SYS_CDEFS_H may be used to include <sys/cdefs.h> both when
  dnl it has been checked for via the GNULIB configure test and found and when
  dnl it has not been checked for, which we can presume means that the <glob.h>
  dnl GNULIB shares with GLIBC is being included as a system header and not as
  dnl part of GNULIB, in which case <sys/cdefs.h> may be assumed.
  if test $SYS_CDEFS_H = no; then
    AC_DEFINE(_SYS_CDEFS_H, 1,
      [Define to `1' if <sys/cdefs.h> is *not* available on this system.])
  fi
  :])
