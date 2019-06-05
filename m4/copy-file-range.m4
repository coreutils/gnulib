# copy-file-range.m4
dnl Copyright 2019 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_COPY_FILE_RANGE],
[
  AC_REQUIRE([gl_UNISTD_H_DEFAULTS])

  dnl Persuade glibc <unistd.h> to declare copy_file_range.
  AC_REQUIRE([AC_USE_SYSTEM_EXTENSIONS])

  AC_CHECK_FUNCS_ONCE([copy_file_range])
  if test $ac_cv_func_copy_file_range != yes; then
    HAVE_COPY_FILE_RANGE=0
  fi
])
