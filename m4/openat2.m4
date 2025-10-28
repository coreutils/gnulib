# openat2.m4
# serial 1

dnl Copyright 2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

# Written by Paul Eggert.

AC_DEFUN([gl_FUNC_OPENAT2],
[
  AC_REQUIRE([gl_FCNTL_H_DEFAULTS])
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])
  AC_REQUIRE([gl_FCNTL_O_FLAGS])
  AC_CHECK_FUNCS_ONCE([openat2])
  AS_CASE([$ac_cv_func_openat2],
    [yes], [HAVE_OPENAT2=1])
])

# Prerequisites of lib/openat2.c.
AC_DEFUN([gl_PREREQ_OPENAT2],
[
  AC_CHECK_FUNCS_ONCE([fstatfs])
  AC_CHECK_HEADERS_ONCE([sys/vfs.h])
  AS_CASE([$ac_cv_func_fstatfs,$ac_cv_header_sys_vfs_h],
    [yes,yes],
    [AC_CHECK_MEMBERS([struct statfs.f_type], [], [],
       [[$ac_includes_default
         #include <sys/vfs.h>
       ]])])
])
