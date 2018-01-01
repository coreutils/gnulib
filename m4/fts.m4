#serial 21
dnl Copyright (C) 2005-2018 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_FTS],
[
  gl_FUNC_FTS_CORE
])

AC_DEFUN([gl_FUNC_FTS_CORE],
[
  dnl Prerequisites of lib/fts.c.
  gl_FUNC_OPENAT

  AC_CHECK_FUNCS_ONCE([fstatfs])
  AC_CHECK_HEADERS_ONCE([sys/param.h sys/vfs.h])dnl
  if test "$ac_cv_func_fstatfs,$ac_cv_header_sys_vfs_h" = yes,yes; then
    AC_CHECK_MEMBERS([struct statfs.f_type], [], [],
      [[$ac_includes_default
        #include <sys/vfs.h>
      ]])
    if test "$ac_cv_member_struct_statfs_f_type" = yes; then
      AC_CHECK_TYPES([__fsword_t], [], [],
        [[$ac_includes_default
          #include <sys/vfs.h>
        ]])
    fi
  fi
])
