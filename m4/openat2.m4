# openat2.m4
# serial 3

dnl Copyright 2025-2026 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

# Written by Paul Eggert.

AC_DEFUN([gl_FUNC_OPENAT2],
[
  AC_REQUIRE([AC_CANONICAL_HOST])
  AC_REQUIRE([gl_FCNTL_H_DEFAULTS])
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])
  AC_REQUIRE([gl_FCNTL_O_FLAGS])
  AC_CHECK_FUNCS_ONCE([openat2])
  AS_CASE([$ac_cv_func_openat2],
    [no], [HAVE_OPENAT2=0],
    [AC_CACHE_CHECK([whether openat2 works with seccomp-bpf],
       [gl_cv_openat2_seccomp_compatible],
       [gl_cv_openat2_seccomp_compatible=yes
        AS_CASE([$host_os],
          [linux*], [gl_cv_openat2_seccomp_compatible=no])])
     AS_CASE([$gl_cv_openat2_seccomp_compatible],
       [no],
         [# Replace openat2 in case seccomp-bpf can break openat2, e.g.:
          # https://github.com/systemd/systemd/issues/44034
          # If targets never use a seccomp-bpf filter that breaks openat2,
          # run './configure gl_cv_openat2_seccomp_compatible=yes'
          # to lessen bloat in apps.
          REPLACE_OPENAT2=1])])
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
