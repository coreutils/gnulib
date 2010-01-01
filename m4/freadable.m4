# freadable.m4 serial 1
dnl Copyright (C) 2007, 2009, 2010 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_FREADABLE],
[
  AC_CHECK_FUNCS_ONCE([__freadable])
  if test $ac_cv_func___freadable = no; then
    AC_LIBOBJ([freadable])
  fi
])
